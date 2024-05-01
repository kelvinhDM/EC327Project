import requests
from datetime import datetime, timedelta
import dotenv
import os
from typing import Optional
import json
import textstat
from app.db import Fact, async_session_maker


class NasaFact:
    copyright: Optional[str]
    date: str
    explanation: str
    hdurl: Optional[str]
    media_type: str
    service_version: str
    title: str
    url: str
    level: int

    def __init__(self, copyright: Optional[str], date: str, explanation: str, hdurl: Optional[str], media_type: str, service_version: str, title: str, url: str) -> None:
        self.copyright = copyright
        self.date = date
        self.explanation = explanation
        self.hdurl = hdurl
        self.media_type = media_type
        self.service_version = service_version
        self.title = title
        self.url = url
        self.level = 0


dotenv.load_dotenv()

API_KEY = os.getenv("NASA_API_KEY")


def download_image(url, path):
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36"
    }
    filename = url.split("/")[-1]
    path = os.path.join(path, filename)
    with open(path, "wb") as file:
        response = requests.get(url, headers=headers)
        file.write(response.content)


def fetch_apod(start_date, end_date):
    url = f"https://api.nasa.gov/planetary/apod?api_key={API_KEY}&start_date={start_date}&end_date={end_date}"
    response = requests.get(url)
    data = response.json()
    return data


def fetch_apod_today():
    url = f"https://api.nasa.gov/planetary/apod?api_key={API_KEY}"
    response = requests.get(url)
    data = response.json()
    if data['media_type'] == "image":
        download_image(data['url'], "imgs")
    return data


def get_normalized_level(grade: float) -> int:
    min_source = 40.69
    max_source = 80.82

    min_target = 1
    max_target = 5
    level = min_target + ((grade - min_source) *
                          (max_target - min_target) / (max_source - min_source))

    return int(round(level))


async def save_fact_to_db(fact: NasaFact):
    async with async_session_maker() as session:
        session.add(Fact(
            date=fact.date,
            title=fact.title,
            media_type=fact.media_type,
            url=fact.url,
            explanation=fact.explanation,
            level=fact.level
        ))
        await session.commit()


async def fetch_apod_today_save_db():
    data = fetch_apod_today()
    fact = NasaFact(
        data.get("copyright"),
        data.get("date"),
        data.get("explanation"),
        data.get("hdurl"),
        data.get("media_type"),
        data.get("service_version"),
        data.get("title"),
        data.get("url")
    )
    grade = textstat.flesch_reading_ease(fact.explanation)
    level = get_normalized_level(grade)
    fact.level = level
    await save_fact_to_db(fact)


async def fetch_last_multi_days_save_db():
    end_date = datetime.now() - timedelta(days=1)
    start_date = end_date - timedelta(days=101)
    start_date = start_date.strftime("%Y-%m-%d")
    end_date = end_date.strftime("%Y-%m-%d")

    data = fetch_apod(start_date, end_date)
    facts = []
    for fact in data:
        facts.append(NasaFact(
            fact.get("copyright"),
            fact.get("date"),
            fact.get("explanation"),
            fact.get("hdurl"),
            fact.get("media_type"),
            fact.get("service_version"),
            fact.get("title"),
            fact.get("url"),
        ))
    for fact in facts:
        grade = textstat.flesch_reading_ease(fact.explanation)
        level = get_normalized_level(grade)
        fact.level = level
        if fact.media_type == "image":
            download_image(fact.url, "imgs")
        await save_fact_to_db(fact)


async def fetch_apod_from_json_save_db():
    with open("facts.json", "r") as file:
        data = json.load(file)
        facts = []
        for fact in data:
            facts.append(NasaFact(
                fact.get("copyright"),
                fact.get("date"),
                fact.get("explanation"),
                fact.get("hdurl"),
                fact.get("media_type"),
                fact.get("service_version"),
                fact.get("title"),
                fact.get("url"),
            ))
        for fact in facts:
            grade = textstat.flesch_reading_ease(fact.explanation)
            level = get_normalized_level(grade)
            fact.level = level
            await save_fact_to_db(fact)
