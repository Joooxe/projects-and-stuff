CREATE TABLE IF NOT EXISTS "teachers"
(
    "teacher_id"        integer PRIMARY KEY,
    "email"             varchar UNIQUE,
    "password_hash"     varchar        NOT NULL,
    "full_name"         varchar,
    "registration_date" timestamp,
    "status"            varchar CHECK ("status" IN ('active', 'inactive', 'blocked'))
);

CREATE TABLE IF NOT EXISTS "students"
(
    "student_id"        integer PRIMARY KEY,
    "email"             varchar UNIQUE,
    "password_hash"     varchar        NOT NULL,
    "full_name"         varchar,
    "registration_date" timestamp,
    "status"            varchar CHECK ("status" IN ('active', 'inactive', 'blocked'))
);

CREATE TABLE IF NOT EXISTS "courses"
(
    "course_sk"   integer PRIMARY KEY,
    "course_id"   integer NOT NULL,
    "teacher_id"  integer NOT NULL,
    "title"       varchar NOT NULL,
    "description" text,
    "valid_from"  date    NOT NULL,
    "valid_to"    date    NOT NULL DEFAULT '9999-12-31',
    "status"      varchar CHECK ("status" IN ('active', 'archived', 'draft'))
);

CREATE TABLE IF NOT EXISTS "enrollments"
(
    "enrollment_id"   integer PRIMARY KEY,
    "course_sk"       integer NOT NULL,
    "student_id"      integer NOT NULL,
    "enrollment_date" date,
    "status"          varchar CHECK ("status" IN ('active', 'completed'))
);

CREATE TABLE IF NOT EXISTS "grades"
(
    "grade_id"      integer PRIMARY KEY,
    "enrollment_id" integer UNIQUE NOT NULL,
    "grade_value"   int,
    "grade_date"    timestamp,
    "comments"      text
);

CREATE TABLE IF NOT EXISTS "lessons"
(
    "lesson_id"   integer PRIMARY KEY,
    "course_sk"   integer NOT NULL,
    "title"       varchar NOT NULL,
    "content"     text,
    "order_index" int
);

CREATE TABLE IF NOT EXISTS "assignments"
(
    "assignment_id" integer PRIMARY KEY,
    "lesson_id"     integer NOT NULL,
    "title"         varchar NOT NULL,
    "description"   text,
    "due_date"      date,
    "max_score"     int
);

CREATE TABLE IF NOT EXISTS "submissions"
(
    "submission_id"  integer PRIMARY KEY,
    "assignment_id"  integer NOT NULL,
    "student_id"     integer NOT NULL,
    "submitted_date" timestamp,
    "content"        text,
    "score"          int,
    "feedback"       text,
    "status"         varchar CHECK ("status" IN ('on review', 'submitted', 'late'))
);

ALTER TABLE "courses"
    ADD FOREIGN KEY ("teacher_id") REFERENCES "teachers" ("teacher_id");

ALTER TABLE "enrollments"
    ADD FOREIGN KEY ("course_sk") REFERENCES "courses" ("course_sk");

ALTER TABLE "enrollments"
    ADD FOREIGN KEY ("student_id") REFERENCES "students" ("student_id");

ALTER TABLE "grades"
    ADD FOREIGN KEY ("enrollment_id") REFERENCES "enrollments" ("enrollment_id");

ALTER TABLE "lessons"
    ADD FOREIGN KEY ("course_sk") REFERENCES "courses" ("course_sk");

ALTER TABLE "assignments"
    ADD FOREIGN KEY ("lesson_id") REFERENCES "lessons" ("lesson_id");

ALTER TABLE "submissions"
    ADD FOREIGN KEY ("assignment_id") REFERENCES "assignments" ("assignment_id");

ALTER TABLE "submissions"
    ADD FOREIGN KEY ("student_id") REFERENCES "students" ("student_id");
