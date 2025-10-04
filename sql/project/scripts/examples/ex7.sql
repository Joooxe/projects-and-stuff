SELECT l.title AS lesson_title,
       COUNT(a.assignment_id) AS total_tasks
FROM lessons l
         JOIN assignments a ON l.lesson_id = a.lesson_id
GROUP BY l.title
HAVING COUNT(a.assignment_id) < 2
ORDER BY total_tasks DESC;
