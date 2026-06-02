#nullable disable
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using InternetShop.Data;
using InternetShop.Models;
using System.Text.Json;

namespace InternetShop.Controllers
{
    public class OrderController : Controller
    {
        private readonly ApplicationDbContext _context;

        public OrderController(ApplicationDbContext context)
        {
            _context = context;
        }

        // GET: /Order/Checkout
        public async Task<IActionResult> Checkout()
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null)
            {
                return RedirectToAction("Login", "Account");
            }

            var cartJson = HttpContext.Session.GetString("Cart");
            if (string.IsNullOrEmpty(cartJson))
            {
                return RedirectToAction("Index", "Cart");
            }

            var cart = JsonSerializer.Deserialize<List<CartItem>>(cartJson) ?? new List<CartItem>();

            // Получаем адрес пользователя из базы
            var user = await _context.Users.FindAsync(userId);
            ViewBag.UserAddress = user?.Address ?? "";

            return View(cart);
        }

        // POST: /Order/Confirm
        [HttpPost]
        public async Task<IActionResult> Confirm(string address)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null)
            {
                return RedirectToAction("Login", "Account");
            }

            var cartJson = HttpContext.Session.GetString("Cart");
            if (string.IsNullOrEmpty(cartJson))
            {
                return RedirectToAction("Index", "Cart");
            }

            var cart = JsonSerializer.Deserialize<List<CartItem>>(cartJson) ?? new List<CartItem>();

            if (!cart.Any())
            {
                return RedirectToAction("Index", "Cart");
            }

            // Проверка наличия товаров
            foreach (var item in cart)
            {
                var product = await _context.Products.FindAsync(item.ProductId);
                if (product == null || product.Stock < item.Quantity)
                {
                    TempData["Error"] = $"Товара \"{item.ProductName}\" нет в наличии в нужном количестве";
                    return RedirectToAction("Checkout");
                }
            }

            // Создание заказа
            var order = new Order
            {
                UserId = userId.Value,
                OrderDate = DateTime.Now,
                Status = "New",
                TotalAmount = cart.Sum(i => i.Total),
                Address = address
            };

            _context.Orders.Add(order);
            await _context.SaveChangesAsync();

            // Добавление позиций заказа
            foreach (var item in cart)
            {
                _context.OrderItems.Add(new OrderItem
                {
                    OrderId = order.Id,
                    ProductId = item.ProductId,
                    Quantity = item.Quantity,
                    PriceAtOrder = item.Price
                });
            }

            await _context.SaveChangesAsync();

            // Очистка корзины
            HttpContext.Session.Remove("Cart");
            HttpContext.Session.SetInt32("CartCount", 0);

            TempData["Success"] = $"Заказ №{order.Id} успешно оформлен!";
            return RedirectToAction("MyOrders");
        }

        // GET: /Order/MyOrders
        public async Task<IActionResult> MyOrders()
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null)
            {
                return RedirectToAction("Login", "Account");
            }

            var orders = await _context.Orders
                .Include(o => o.OrderItems)
                .ThenInclude(oi => oi.Product)
                .Where(o => o.UserId == userId)
                .OrderByDescending(o => o.OrderDate)
                .ToListAsync();

            return View(orders);
        }

        // POST: /Order/CancelOrder
        [HttpPost]
        public async Task<IActionResult> CancelOrder(int orderId)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null)
            {
                return RedirectToAction("Login", "Account");
            }

            var order = await _context.Orders.FindAsync(orderId);
            if (order == null || order.UserId != userId)
            {
                TempData["Error"] = "Заказ не найден";
                return RedirectToAction("MyOrders");
            }

            if (order.Status != "New")
            {
                TempData["Error"] = "Нельзя отменить заказ, который уже обработан";
                return RedirectToAction("MyOrders");
            }

            order.Status = "Cancelled";
            order.RejectReason = "Отменен пользователем";
            await _context.SaveChangesAsync();

            TempData["Success"] = $"Заказ №{orderId} отменен!";
            return RedirectToAction("MyOrders");
        }
    }
}