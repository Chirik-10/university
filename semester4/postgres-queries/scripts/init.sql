-- Таблица поставщиков
CREATE TABLE s (
    n_post CHAR(2) PRIMARY KEY,
    fam VARCHAR(20) NOT NULL,
    rating INT,
    city VARCHAR(20)
);

-- Таблица деталей
CREATE TABLE p (
    n_det CHAR(2) PRIMARY KEY,
    name VARCHAR(20) NOT NULL,
    color VARCHAR(10),
    weight INT,
    city VARCHAR(20)
);

-- Таблица поставок (связь многие-ко-многим)
CREATE TABLE sp (
    n_post CHAR(2),
    n_det CHAR(2),
    quantity INT,
    date DATE,
    PRIMARY KEY (n_post, n_det, date),
    FOREIGN KEY (n_post) REFERENCES s(n_post) ON DELETE CASCADE,
    FOREIGN KEY (n_det) REFERENCES p(n_det) ON DELETE CASCADE
);

-- Поставщики (таблица s)
INSERT INTO s VALUES
('S1', 'Смит',   20, 'Лондон'),
('S2', 'Джонс',  10, 'Париж'),
('S3', 'Блейк',  30, 'Париж'),
('S4', 'Кларк',  20, 'Лондон'),
('S5', 'Адамс',  30, 'Афины');

-- Детали (таблица p)
INSERT INTO p VALUES
('P1', 'Гайка',   'Красный', 12, 'Лондон'),
('P2', 'Болт',    'Зеленый', 17, 'Париж'),
('P3', 'Винт',    'Голубой', 17, 'Рим'),
('P4', 'Винт',    'Красный', 14, 'Лондон'),
('P5', 'Кулачок', 'Голубой', 12, 'Париж'),
('P6', 'Блок',    'Красный', 19, 'Лондон');

-- Поставки (таблица sp)
INSERT INTO sp VALUES
('S1', 'P1', 300, '1995-01-10'),
('S1', 'P2', 200, '1995-01-10'),
('S1', 'P3', 400, '1995-01-20'),
('S1', 'P4', 200, '1995-01-20'),
('S1', 'P5', 100, '1995-01-30'),
('S1', 'P6', 100, '1995-01-30'),
('S2', 'P1', 300, '1995-01-10'),
('S2', 'P2', 400, '1995-01-10'),
('S3', 'P2', 200, '1995-01-20'),
('S4', 'P4', 300, '1995-01-20'),
('S4', 'P5', 400, '1995-01-30'),
('S5', 'P2', 200, '1995-01-10');

-- Проверка заполнения
SELECT 'Таблица s:' as info;
SELECT * FROM s;

SELECT 'Таблица p:' as info;
SELECT * FROM p;

SELECT 'Таблица sp:' as info;
SELECT * FROM sp;
