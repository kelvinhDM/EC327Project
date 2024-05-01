from typing import AsyncGenerator

from sqlalchemy.ext.asyncio import AsyncSession, async_sessionmaker, create_async_engine
from sqlalchemy.orm import DeclarativeBase, Mapped
from sqlalchemy import Column, Integer, String
DATABASE_URL = "sqlite+aiosqlite:///./facts.db"


class Base(DeclarativeBase):
    pass


class Fact(Base):
    __tablename__ = "fact"

    id: Mapped[int] = Column(Integer, primary_key=True, index=True)
    date: Mapped[str] = Column(String, index=True, unique=True)
    title: Mapped[str] = Column(String, index=True)
    media_type: Mapped[str] = Column(String)
    url: Mapped[str] = Column(String, index=True)
    explanation: Mapped[str] = Column(String)
    # 1~5
    level: Mapped[int] = Column(Integer, index=True)


class Ranking(Base):
    __tablename__ = "ranking"

    id: Mapped[int] = Column(Integer, primary_key=True, index=True)
    username: Mapped[str] = Column(String, index=True)
    score: Mapped[int] = Column(Integer, index=True)
    created_at: Mapped[str] = Column(String, index=True)


class ReadProgress(Base):
    __tablename__ = "visited_fact"

    id: Mapped[int] = Column(Integer, primary_key=True, index=True)
    level: Mapped[int] = Column(Integer, index=True)
    username: Mapped[str] = Column(String, index=True, unique=True)
    count: Mapped[int] = Column(Integer, index=True)


engine = create_async_engine(DATABASE_URL)
async_session_maker = async_sessionmaker(engine, expire_on_commit=False)


async def create_db_and_tables():
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)


async def get_async_session() -> AsyncGenerator[AsyncSession, None]:
    async with async_session_maker() as session:
        yield session
