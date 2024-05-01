import os
from datetime import datetime
from fastapi.responses import FileResponse
from tabulate import tabulate
from textwrap import fill
from dotenv import load_dotenv

from fastapi import Depends, FastAPI, HTTPException, Response

from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession
from app.crawler import fetch_apod_today_save_db
from app.db import Fact, Ranking, ReadProgress, create_db_and_tables, get_async_session
from app.schemas import FactRead, RankingRead
from fastapi.middleware.cors import CORSMiddleware

from fastapi_pagination import Page, add_pagination, paginate
from apscheduler.schedulers.asyncio import AsyncIOScheduler
from fastapi_pagination.utils import disable_installed_extensions_check

disable_installed_extensions_check()
load_dotenv()

app = FastAPI()
add_pagination(app)
origins = ["*"]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

image_directory = "imgs"


@app.get("/images/{filename}")
def get_image(filename: str):
    image_path = os.path.join(image_directory, filename)

    if not os.path.isfile(image_path):
        # If the file does not exist, return a 404 error
        raise HTTPException(status_code=404, detail="Image not found")

    return FileResponse(image_path)


@app.get("/facts/", response_model=Page[FactRead], tags=["facts"])
async def get_facts(session: AsyncSession = Depends(get_async_session)):
    async with session:
        result = await session.execute(select(Fact))
        facts = result.scalars().all()
        return paginate(facts)


@app.get("/facts/search-by-title/", response_model=Page[FactRead], tags=["facts"])
async def search_facts_by_title(title: str, session: AsyncSession = Depends(get_async_session)):
    async with session:
        query = select(Fact).filter(Fact.title.ilike(f"%{title}%"))
        result = await session.execute(query)
        facts = result.scalars().all()
        return paginate(facts)


@app.get("/facts/search-by-level/", response_model=Page[FactRead], tags=["facts"])
async def search_facts_by_level(level: int, session: AsyncSession = Depends(get_async_session)):
    async with session:
        query = select(Fact).filter(Fact.level == level)
        result = await session.execute(query)
        facts = result.scalars().all()
        return paginate(facts)


@app.get("/facts/get-one-by-username/", response_model=str, tags=["fact"])
async def get_one_fact_by_username(username: str, session: AsyncSession = Depends(get_async_session)):
    async with session:
        query = select(ReadProgress).filter(ReadProgress.username == username)
        result = await session.execute(query)
        progress = result.scalars().first()

        if progress:
            query = select(Fact).filter(Fact.level == progress.level)
            result = await session.execute(query)
            facts = result.scalars().all()

            # If the user has read all the facts in the level, increase the level
            if progress.count >= len(facts):
                if progress.level < 5:
                    progress.level += 1
                    progress.count = 0
                else:
                    # If all facts are read (max level is 5), reset the level to 1
                    progress.level = 1
                    progress.count = 0
                await session.commit()

            # Get the first unread fact in the current level
            query = select(Fact).filter(Fact.level == progress.level).order_by(
                Fact.id).offset(progress.count).limit(1)
            result = await session.execute(query)
            fact = result.scalars().first()

            # Increment the count of read facts for the user
            progress.count += 1
            await session.commit()
        else:
            # If no progress found, create a new progress entry with level 1
            progress = ReadProgress(username=username, level=1, count=1)
            session.add(progress)
            await session.commit()

            # Get the first fact in level 1
            query = select(Fact).filter(
                Fact.level == 1).order_by(Fact.id).limit(1)
            result = await session.execute(query)
            fact = result.scalars().first()

        if fact:
            width = 60
            wrapped_title = fill(fact.title, width=width)
            wrapped_explanation = fill(fact.explanation, width=width)
            data = [
                ["Title", f'{wrapped_title} ({fact.level})'],
                ["Fact", wrapped_explanation]
            ]
            table_str = tabulate(data, tablefmt="grid")
            if fact.media_type == "image":
                filename = fact.url.split("/")[-1]
                table_str += f'IMAGE_URL{filename}'
            return Response(content=table_str, media_type="text/plain")
        else:
            raise HTTPException(
                status_code=404, detail="No fact found for the given level")


@app.post("/rankings/", response_model=RankingRead, tags=["rankings"])
async def create_ranking(username: str, score: int, session: AsyncSession = Depends(get_async_session)):
    async with session:
        created_at = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        ranking = Ranking(username=username, score=score,
                          created_at=created_at)
        session.add(ranking)
        await session.commit()
        await session.refresh(ranking)
        return ranking


@app.get("/rankings/", response_model=str, tags=["rankings"])
async def get_rankings(session: AsyncSession = Depends(get_async_session)):
    async with session:
        query = select(Ranking).order_by(Ranking.score.desc()).limit(5)
        result = await session.execute(query)
        rankings = result.scalars().all()
        # Prepare the data for tabulate
        table = [[i + 1, ranking.username, ranking.score, ranking.created_at]
                 for i, ranking in enumerate(rankings)]
        # Create headers
        headers = ["TOP 5", "Username", "Score", "Date"]
        # Return a beautiful table
        rankings_str = tabulate(table, headers, tablefmt="grid")
        return Response(content=rankings_str, media_type="text/plain")


@app.delete("/rankings/{ranking_id}", response_model=None, tags=["rankings"])
async def delete_ranking(ranking_id: int, session: AsyncSession = Depends(get_async_session)):
    async with session:
        query = select(Ranking).filter(Ranking.id == ranking_id)
        result = await session.execute(query)
        ranking = result.scalars().first()

        if ranking:
            await session.delete(ranking)
            await session.commit()
        else:
            raise HTTPException(status_code=404, detail="Ranking not found")


@app.on_event("startup")
async def on_startup():
    await create_db_and_tables()
    scheduler = AsyncIOScheduler()
    # fetch every day at 10:00
    scheduler.add_job(fetch_apod_today_save_db, 'cron',
                      hour=10, minute=0)
    scheduler.start()
