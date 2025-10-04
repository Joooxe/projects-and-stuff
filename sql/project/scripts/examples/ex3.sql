SELECT t.full_name AS teacher_name
FROM teachers t
WHERE t.status = 'active'
  AND t.teacher_id NOT IN (SELECT c.teacher_id
                           FROM courses c
                           WHERE c.status = 'archived');
