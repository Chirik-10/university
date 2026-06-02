# Проект "Интернет-магазин" (веб-приложение)

## О проекте

Полнофункциональный веб-сервис интернет-магазина, разработанный в рамках курса. Реализованы все ключевые сценарии использования: регистрация, поиск товаров, управление ассортиментом (админ), обработка заказов, корзина с динамическими обновлениями через AJAX.

## Реализованные сценарии использования

| № | Сценарий | Описание |
|---|----------|----------|
| 1 | Регистрация | Создание учётной записи с проверкой уникальности email |
| 2 | Поиск товара | Поиск по названию + фильтрация по категории и цене |
| 3 | Добавление товара (админ) | Форма с валидацией, загрузка изображения |
| 4 | Обработка заказа (админ) | Подтверждение/отклонение заказов, списание/возврат товаров |
| 5 | Оформление заказа | Корзина с количеством, оформление с указанием адреса |

## Технологический стек
- Backend: ASP.NET Core (.NET 6/7/8), C#
- Database: MS SQL Server + Entity Framework Core (Code First)
- Frontend: HTML5, CSS3, Bootstrap, JavaScript, jQuery
- AJAX: Асинхронные запросы для улучшения UX
- Auth: Сессии, хеширование паролей (SHA256)
- Validation: Client-side + Server-side

## AJAX-функционал

| Функция | Тип | Описание |
|---------|-----|----------|
| Проверка email | POST | Мгновенная проверка уникальности email при регистрации |
| Проверка паролей | клиент | Совпадение паролей в реальном времени |
| Фильтрация товаров | GET | Обновление каталога без перезагрузки страницы |
| Изменение количества в корзине | POST | Кнопки "+/-" с асинхронным обновлением |
| Удаление товара из корзины | POST | Удаление с подтверждением |

## База данных (Entity Framework Core)

Сущности:

- Users - пользователи (Id, Name, Email, Password, Role, Address, AvatarPath)
- Categories - категории товаров
- Products - товары (Name, Price, Stock, CategoryId, Image)
- Orders - заказы (UserId, OrderDate, Status, TotalAmount, Address)
- OrderItems - позиции заказа (Quantity, PriceAtOrder)

Связи:

- User -> Orders (1:N)
- Category -> Products (1:N)
- Order -> OrderItems (1:N)
- Product -> OrderItems (1:N)

## Скриншоты интерфейса

### Главная страница (каталог + поиск + фильтры)
<img width="1891" height="865" alt="image" src="https://github.com/user-attachments/assets/882fe3b2-afe9-496d-af01-abc7cfd98353" />

### Корзина (динамическое изменение количества)
<img width="1920" height="757" alt="image" src="https://github.com/user-attachments/assets/042de7dd-9134-49b1-9ad5-e4e67871aa67" />

### Окно регистрации
<img width="1920" height="825" alt="image" src="https://github.com/user-attachments/assets/fad1cc72-6802-42c6-b772-c0419d277639" />

### Админ-панель: управление товарами
<img width="1900" height="830" alt="image" src="https://github.com/user-attachments/assets/10608729-82d3-411c-bc44-6379fa6f53af" />

### Админ-панель: управление заказами
<img width="1892" height="845" alt="image" src="https://github.com/user-attachments/assets/b44f08bf-5e78-41f4-ad86-ee22d3570786" />

### Окно Мой профиль
<img width="1896" height="847" alt="image" src="https://github.com/user-attachments/assets/aa77b958-47d2-4234-9ee6-50435c35ebd4" />

## Принципы проектирования интерфейса

| Принцип | Реализация |
|---------|-------------|
| Структурный | Шапка (меню), контент, подвал. Единообразные карточки товаров |
| Простота | Добавление в корзину в 1 клик. Поиск на главной странице |
| Видимость | Ключевая информация на карточке. Статус заказа — цветом |
| Обратная связь | Сообщения об успехе/ошибке. Подсветка полей при валидации |
| Толерантность | Проверка цен (отрицательные запрещены). Проверка наличия товара |
| Повторное использование | Единый шаблон _Layout.cshtml для всех страниц |

## Структура проекта
```
InternetShop/
  Controllers/
    AccountController.cs # Регистрация, вход, проверка email 
    AdminController.cs # Управление товарами и заказами
    CartController.cs # Корзина 
    HomeController.cs # Главная страница, поиск, фильтрация 
    OrderController.cs # Оформление заказа, история заказов
    ProfileController.cs # Личный кабинет
  Models/
    CartItem.cs # Карточка товара
    Category.cs # Категория товара
    Order.cs # Заказ
    Product.cs # Товар
    User.cs # Пользователь
  Views/ # Представления
    Account/
      Login.cshtml # Страница входа в аккаунт
      Register.cshtml # Страница регистрации
    Admin/
      CreateProduct.cshtml # Добавление товара
      EditProduct.cshtml # Редактирование товара
      Orders.cshtml # Все заказы
      Products.cshtml # Товары
    Cart/
      Index.cshtml # Корзина
    Home/
      Index.cshtml # Главная страница
      _ProductsGrid.cshtml # Отображение товаров
    Order/
      Checkout.cshtml # Страница оформления заказа
      MyOrder.cshtml # История заказов пользователя
    Profil/
      Index.cshtml # Профиль
    Shared/
      _Layout.cshtml # Единый шаблон
    _ViewStart.cshtml # Настройка Layout по умолчанию
  wwwroot/
    css/
      site.css # Стиль сайта
    images/ # Аватары пользователей, иконки товаров
    js/
      site.js
  appsettings.json # Подключение к БД
  Program.cs # Точка входа
```
