CREATE TABLE IF NOT EXISTS animals (
    id SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    species VARCHAR(50),
    age INT,
    gender VARCHAR(10),
    enclosure VARCHAR(50)
);

INSERT INTO animals (name, species, age, gender, enclosure) VALUES
('Лео', 'Лев', 5, 'Самец', 'Африка'),
('Зара', 'Жираф', 7, 'Самка', 'Африка'),
('Мила', 'Пингвин', 3, 'Самка', 'Антарктида');

