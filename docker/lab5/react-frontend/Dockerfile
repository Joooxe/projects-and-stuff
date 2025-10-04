FROM node:16-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci --production && npm cache clean --force
COPY public ./public
COPY src ./src
COPY .env ./
RUN npm run build

FROM node:16-alpine
RUN addgroup -S app && adduser -S app -G app \
    && apk add --no-cache curl
WORKDIR /app
COPY --from=builder /app/build ./build
RUN npm install -g serve && npm cache clean --force
USER app
ENTRYPOINT serve -s build -l tcp://0.0.0.0:${FRONTEND_PORT}
