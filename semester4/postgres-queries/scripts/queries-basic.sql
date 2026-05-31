-- Выбор всех строк и столбцов
\echo '1. Полная информация о поставщиках:';
SELECT * FROM s;

\echo '2. Полная информация о деталях:';
SELECT * FROM p;

-- Выбор заданных столбцов
\echo '3. Номера всех поставляемых деталей:';
SELECT n_det FROM sp;

\echo '4. Номера всех поставщиков:';
SELECT n_post FROM sp;

-- Выбор без повторений (DISTINCT)
\echo '5. Номера поставляемых деталей (без дублей):';
SELECT DISTINCT n_det FROM sp;

\echo '6. Номера поставщиков (без дублей):';
SELECT DISTINCT n_post FROM sp;

-- Ограничение выборки (WHERE)
\echo '7. Поставщики из Парижа с рейтингом > 20:';
SELECT n_post, fam, rating, city
FROM s
WHERE city = 'Париж' AND rating > 20;

\echo '8. Детали с весом от 15 до 18 (BETWEEN):';
SELECT * FROM p
WHERE weight BETWEEN 15 AND 18;

\echo '9. Поставщики с рейтингом 10 или 30 (IN):';
SELECT * FROM s
WHERE rating IN (10, 30);

-- Выбор по шаблону (LIKE)
\echo '10. Детали, начинающиеся на "В":';
SELECT * FROM p
WHERE name LIKE 'В%';

\echo '11. Поставщики, у которых вторая буква фамилии "л":';
SELECT fam FROM s
WHERE fam LIKE '_л%';

-- Сортировка (ORDER BY)
\echo '12. Поставщики из Парижа по убыванию рейтинга:';
SELECT n_post, fam, rating
FROM s
WHERE city = 'Париж'
ORDER BY rating DESC;

\echo '13. Детали красного цвета, упорядоченные по весу:';
SELECT n_det, name, weight
FROM p
WHERE color = 'Красный'
ORDER BY weight;
