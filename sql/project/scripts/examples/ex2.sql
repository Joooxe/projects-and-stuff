SELECT c.course_id,
       c.title                AS course_title,
       COUNT(e.enrollment_id) AS total_enrollments
FROM courses c
         JOIN enrollments e ON c.course_sk = e.course_sk
GROUP BY c.course_id, c.title
HAVING COUNT(e.enrollment_id) >= 5
ORDER BY total_enrollments DESC;
