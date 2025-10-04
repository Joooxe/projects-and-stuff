SELECT s1.full_name AS nameA,
       s2.full_name AS nameB,
       s1.registration_date AS regA,
       s2.registration_date AS regB
FROM students s1
         JOIN students s2
              ON s1.student_id < s2.student_id
                  AND s2.registration_date BETWEEN s1.registration_date - INTERVAL '1 day'
                     AND s1.registration_date + INTERVAL '1 day'
ORDER BY regA, regB;
