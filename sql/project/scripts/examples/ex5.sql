SELECT c.course_id,
       s.full_name   AS student_name,
       g.grade_value AS final_grade,
       RANK() OVER (
           PARTITION BY c.course_id
           ORDER BY g.grade_value DESC
           ) AS rank_in_course
FROM grades g
         JOIN enrollments e ON g.enrollment_id = e.enrollment_id
         JOIN students s    ON e.student_id    = s.student_id
         JOIN courses c     ON e.course_sk     = c.course_sk
ORDER BY c.course_id, rank_in_course;
