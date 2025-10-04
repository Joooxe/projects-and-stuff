FROM python:3.11-alpine AS builder

WORKDIR /app

RUN apk add --no-cache --update \
    gcc \
    musl-dev \
    postgresql-dev \
    && pip install --no-cache-dir --upgrade pip setuptools wheel

COPY requirements.txt .
RUN pip install --prefix=/install --no-cache-dir -r requirements.txt


FROM python:3.11-alpine

ENV PYTHONUNBUFFERED=1
WORKDIR /app

RUN apk add --no-cache --update postgresql-libs \
    && adduser -D django \
    && mkdir -p /app/static \
    && chown django /app/static

USER django

COPY --from=builder /install /usr/local
COPY . /app

EXPOSE 5000

CMD ["python", "manage.py", "runserver", "0.0.0.0:5000"]

