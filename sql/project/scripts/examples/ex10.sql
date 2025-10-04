WITH naruto_id AS (
    SELECT student_id
    FROM students
    WHERE full_name = 'Naruto Uzumaki'
)
SELECT s.full_name AS student_name
FROM students s
         JOIN enrollments e ON s.student_id = e.student_id
         JOIN grades g ON e.enrollment_id = g.enrollment_id
WHERE g.grade_value > ALL (
    SELECT g2.grade_value
    FROM enrollments e2
             JOIN grades g2 ON e2.enrollment_id = g2.enrollment_id
             JOIN naruto_id ni ON e2.student_id = ni.student_id
)
ORDER BY s.full_name;
