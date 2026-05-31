-- Агрегатные функции
\echo '1. Общее количество поставщиков:';
SELECT COUNT(*) as total_suppliers FROM s;

\echo '2. Общее количество деталей:';
SELECT COUNT(*) as total_parts FROM p;

\echo '3. Общее количество поставок для детали P2:';
SELECT SUM(quantity) as total_quantity FROM sp WHERE n_det = 'P2';

\echo '4. Средний, мин и макс объем поставки для поставщика S1:';
SELECT 
    AVG(quantity) as avg_quantity,
    MIN(quantity) as min_quantity,
    MAX(quantity) as max_quantity
FROM sp 
WHERE n_post = 'S1';

-- Группировка (GROUP BY)
\echo '5. Для каждой детали - общий объем поставок:';
SELECT n_det, SUM(quantity) as total_quantity
FROM sp
GROUP BY n_det;

\echo '6. Детали, поставляемые более чем одним поставщиком (HAVING):';
SELECT n_det, COUNT(DISTINCT n_post) as suppliers_count
FROM sp
GROUP BY n_det
HAVING COUNT(DISTINCT n_post) > 1;

-- Соединения (JOIN)
\echo '7. Информация о поставках с фамилиями поставщиков (INNER JOIN):';
SELECT s.n_post, s.fam, sp.n_det, sp.quantity, sp.date
FROM s
JOIN sp ON s.n_post = sp.n_post
LIMIT 10;

\echo '8. Поставщики и детали из одного города (эквисоединение):';
SELECT s.n_post, s.fam, s.city, p.n_det, p.name
FROM s
JOIN p ON s.city = p.city;

\echo '9. Все возможные пары поставщик-деталь (декартово произведение):';
SELECT s.fam, p.name
FROM s, p
LIMIT 15;

-- Подзапросы
\echo '10. Поставщики с рейтингом выше любого лондонского поставщика (ALL):';
SELECT fam, rating
FROM s
WHERE rating > ALL (SELECT rating FROM s WHERE city = 'Лондон');

\echo '11. Поставщики, поставляющие деталь P2 (EXISTS):';
SELECT fam
FROM s
WHERE EXISTS (
    SELECT 1 FROM sp
    WHERE sp.n_post = s.n_post AND sp.n_det = 'P2'
);

\echo '12. Поставщики, которые поставляют ВСЕ детали (двойное отрицание):';
SELECT fam
FROM s
WHERE NOT EXISTS (
    SELECT n_det FROM p
    WHERE NOT EXISTS (
        SELECT 1 FROM sp
        WHERE sp.n_post = s.n_post AND sp.n_det = p.n_det
    )
);

-- Подзапрос с IN
\echo '13. Фамилии поставщиков, поставляющих красные детали:';
SELECT DISTINCT s.fam
FROM s
WHERE s.n_post IN (
    SELECT sp.n_post
    FROM sp
    WHERE sp.n_det IN (
        SELECT p.n_det
        FROM p
        WHERE p.color = 'Красный'
    )
);
