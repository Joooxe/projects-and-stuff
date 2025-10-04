SELECT t.teacher_id,
       t.full_name
FROM teachers t
WHERE NOT EXISTS (SELECT 1
                  FROM courses c
                  WHERE c.teacher_id = t.teacher_id
                    AND c.status = 'active');
