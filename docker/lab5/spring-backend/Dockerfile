FROM maven:3.9-eclipse-temurin-11-alpine AS builder
WORKDIR /app
COPY pom.xml .
COPY src ./src
RUN mvn clean package -DskipTests

FROM openjdk:11-jre-slim

RUN apt-get update \
 && apt-get install -y netcat \
 && rm -rf /var/lib/apt/lists/* \
 && addgroup --system app \
 && adduser --system --ingroup app app

WORKDIR /app
COPY --from=builder /app/target/*.jar app.jar
USER app
ENTRYPOINT ["java", "-jar", "app.jar"]

