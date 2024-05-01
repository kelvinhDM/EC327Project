from pydantic import BaseModel


class FactRead(BaseModel):
    id: int
    date: str
    title: str
    media_type: str
    url: str
    explanation: str
    level: int

    class Config:
        from_attributes = True


class RankingRead(BaseModel):
    id: int
    username: str
    score: int
    created_at: str

    class Config:
        from_attributes = True
