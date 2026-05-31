const express = require('express');
const { Pool } = require('pg');
const bodyParser = require('body-parser');
const path = require('path');

const app = express();

// Подключение к БД
const pool = new Pool({
    host: process.env.DB_HOST,
    user: process.env.DB_USER,
    password: process.env.DB_PASSWORD,
    database: process.env.DB_NAME,
    port: 5432,
});

// Настройки
app.set('view engine', 'ejs');
app.use(express.static(path.join(__dirname, 'public')));
app.use(bodyParser.urlencoded({ extended: false }));

// Главная страница — список животных
app.get('/', async (req, res) => {
    try {
        const result = await pool.query('SELECT * FROM animals');
        res.render('index', { animals: result.rows });
    } catch (err) {
        console.error(err);
        res.status(500).send('Ошибка сервера');
    }
});

// Форма добавления
app.get('/add', (req, res) => {
    res.render('add');
});

// Обработка формы добавления
app.post('/add', async (req, res) => {
    const { name, species, age, gender, enclosure } = req.body;
    try {
        await pool.query(
            'INSERT INTO animals (name, species, age, gender, enclosure) VALUES ($1, $2, $3, $4, $5)',
            [name, species, age, gender, enclosure]
        );
        res.redirect('/');
    } catch (err) {
        console.error(err);
        res.status(500).send('Ошибка при добавлении животного');
    }
});

// Удаление животного
app.post('/delete/:id', async (req, res) => {
    const { id } = req.params;
    try {
        await pool.query('DELETE FROM animals WHERE id = $1', [id]);
        res.redirect('/');
    } catch (err) {
        console.error(err);
        res.status(500).send('Ошибка при удалении');
    }
});

// Запуск сервера
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Сервер запущен на порту ${PORT}`);
});
