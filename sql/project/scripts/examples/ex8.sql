SELECT c.title    AS course_title,
       COUNT(l.lesson_id)                 AS total_lessons
FROM courses c
         FULL JOIN lessons l ON c.course_sk = l.course_sk
GROUP BY c.title
ORDER BY total_lessons ASC;
