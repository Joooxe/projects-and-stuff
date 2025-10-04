SELECT st.full_name      AS student_name,
       c.title           AS course_title,
       e.enrollment_date AS enrolled_on
FROM enrollments e
         JOIN students st ON e.student_id = st.student_id
         JOIN courses c ON e.course_sk = c.course_sk
WHERE st.status = 'active'
  AND c.status = 'active'
ORDER BY e.enrollment_date ASC;
