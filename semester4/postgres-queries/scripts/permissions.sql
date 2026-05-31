-- Создание новых пользователей (ролей)
\echo '1. Создание ролей alex, bob, polina, lena:';
CREATE ROLE alex WITH LOGIN PASSWORD 'alex_pass';
CREATE ROLE bob WITH LOGIN PASSWORD 'bob_pass';
CREATE ROLE polina WITH LOGIN PASSWORD 'polina_pass';
CREATE ROLE lena WITH LOGIN PASSWORD 'lena_pass';

-- Просмотр созданных ролей
\echo '2. Список ролей:';
\du

-- Предоставление прав на создание объектов в базе данных
\echo '3. Права на создание объектов в БД study_sql:';
GRANT CREATE ON DATABASE study_sql TO alex, bob, polina, lena;

-- Предоставление прав на таблицы
\echo '4. Предоставление прав пользователю alex на чтение таблицы s:';
GRANT SELECT ON s TO alex;

-- Предоставление прав на вставку и обновление
\echo '5. Права на вставку в таблицу sp пользователю bob:';
GRANT INSERT ON sp TO bob;

-- Проверка прав (от имени текущего пользователя)
\echo '6. Текущие права на таблицы:';
SELECT grantee, privilege_type 
FROM information_schema.table_privileges 
WHERE table_name = 's';

-- Отзыв прав
\echo '7. Отзыв права SELECT у alex:';
REVOKE SELECT ON s FROM alex;

-- Удаление ролей
\echo '8. Удаление ролей:';
-- DROP ROLE IF EXISTS alex;
-- DROP ROLE IF EXISTS bob;
-- DROP ROLE IF EXISTS polina;
-- DROP ROLE IF EXISTS lena;

\echo 'psql -U alex -d study_sql';
