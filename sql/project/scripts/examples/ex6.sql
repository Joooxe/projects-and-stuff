WITH student_grades AS (
    SELECT
        e.student_id,
        g.grade_value
    FROM grades g
             JOIN enrollments e ON g.enrollment_id = e.enrollment_id
) -- на самом деле можно 3 жойна бахнуть, но нужно какое-то разнообразие
SELECT
    st.student_id,
    st.full_name,
    AVG(sg.grade_value) OVER (
        PARTITION BY st.student_id
        ORDER BY st.student_id
        ) AS avg_grade
FROM students st
         LEFT JOIN student_grades sg ON st.student_id = sg.student_id
ORDER BY avg_grade DESC NULLS LAST
LIMIT 5;
