#nullable disable
using Microsoft.AspNetCore.Mvc;
using InternetShop.Models;
using InternetShop.Data;
using System.Text.Json;

namespace InternetShop.Controllers
{
    public class CartController : Controller
    {
        private readonly ApplicationDbContext _context;
        private const string CartSessionKey = "Cart";

        public CartController(ApplicationDbContext context)
        {
            _context = context;
        }

        private List<CartItem> GetCart()
        {
            var cartJson = HttpContext.Session.GetString(CartSessionKey);
            if (string.IsNullOrEmpty(cartJson))
            {
                return new List<CartItem>();
            }
            return JsonSerializer.Deserialize<List<CartItem>>(cartJson) ?? new List<CartItem>();
        }

        private void SaveCart(List<CartItem> cart)
        {
            var cartJson = JsonSerializer.Serialize(cart);
            HttpContext.Session.SetString(CartSessionKey, cartJson);
            HttpContext.Session.SetInt32("CartCount", cart.Sum(i => i.Quantity));
        }

        public IActionResult Index()
        {
            var cart = GetCart();
            return View(cart);
        }

        [HttpPost]
        public IActionResult AddToCart(int productId, string productName, decimal price, string image, int stock)
        {
            var cart = GetCart();
            var existingItem = cart.FirstOrDefault(i => i.ProductId == productId);

            if (existingItem != null)
            {
                if (existingItem.Quantity < stock)
                {
                    existingItem.Quantity++;
                    TempData["Success"] = "Товар добавлен в корзину";
                }
                else
                {
                    TempData["Error"] = $"Нельзя добавить больше {stock} шт. (доступно на складе)";
                }
            }
            else
            {
                if (stock > 0)
                {
                    cart.Add(new CartItem
                    {
                        ProductId = productId,
                        ProductName = productName ?? "Товар",
                        Price = price,
                        Quantity = 1,
                        Image = image ?? "default.png"
                    });
                    TempData["Success"] = "Товар добавлен в корзину";
                }
                else
                {
                    TempData["Error"] = "Товара нет в наличии";
                }
            }

            SaveCart(cart);
            return Redirect(Request.Headers["Referer"].ToString() ?? "/");
        }

        [HttpPost]
        public IActionResult IncreaseQuantity(int productId, int maxStock)
        {
            var cart = GetCart();
            var item = cart.FirstOrDefault(i => i.ProductId == productId);

            if (item != null)
            {
                if (item.Quantity < maxStock)
                {
                    item.Quantity++;
                    SaveCart(cart);
                    return Json(new { success = true, newQuantity = item.Quantity });
                }
                else
                {
                    return Json(new { success = false, message = $"Нельзя заказать больше {maxStock} шт." });
                }
            }

            return Json(new { success = false, message = "Товар не найден" });
        }

        [HttpPost]
        public IActionResult DecreaseQuantity(int productId)
        {
            var cart = GetCart();
            var item = cart.FirstOrDefault(i => i.ProductId == productId);

            if (item != null)
            {
                if (item.Quantity > 1)
                {
                    item.Quantity--;
                    SaveCart(cart);
                    return Json(new { success = true, newQuantity = item.Quantity });
                }
                else
                {
                    cart.Remove(item);
                    SaveCart(cart);
                    return Json(new { success = true, removed = true });
                }
            }

            return Json(new { success = false, message = "Товар не найден" });
        }

        [HttpPost]
        public IActionResult RemoveFromCart(int productId)
        {
            var cart = GetCart();
            var item = cart.FirstOrDefault(i => i.ProductId == productId);

            if (item != null)
            {
                cart.Remove(item);
                SaveCart(cart);
                return Json(new { success = true, message = "Товар удален" });
            }

            return Json(new { success = false, message = "Товар не найден" });
        }

        // AJAX: добавление в корзину без перезагрузки
        [HttpPost]
        public async Task<IActionResult> AddToCartAjax(int productId)
        {
            var product = await _context.Products.FindAsync(productId);
            if (product == null)
            {
                return Json(new { success = false, message = "Товар не найден" });
            }

            var cart = GetCart();
            var existingItem = cart.FirstOrDefault(i => i.ProductId == productId);

            if (existingItem != null)
            {
                if (existingItem.Quantity < product.Stock)
                {
                    existingItem.Quantity++;
                }
                else
                {
                    return Json(new { success = false, message = $"Нельзя добавить больше {product.Stock} шт." });
                }
            }
            else
            {
                if (product.Stock > 0)
                {
                    cart.Add(new CartItem
                    {
                        ProductId = product.Id,
                        ProductName = product.Name,
                        Price = product.Price,
                        Quantity = 1,
                        Image = product.Image
                    });
                }
                else
                {
                    return Json(new { success = false, message = "Товара нет в наличии" });
                }
            }

            SaveCart(cart);
            return Json(new { success = true, cartCount = cart.Sum(i => i.Quantity), message = "Товар добавлен в корзину" });
        }

        // AJAX: обновление количества
        [HttpPost]
        public IActionResult UpdateQuantityAjax(int productId, int quantity)
        {
            var cart = GetCart();
            var item = cart.FirstOrDefault(i => i.ProductId == productId);

            if (item != null)
            {
                if (quantity <= 0)
                {
                    cart.Remove(item);
                }
                else
                {
                    item.Quantity = quantity;
                }
                SaveCart(cart);

                var cartTotal = cart.Sum(i => i.Price * i.Quantity);
                var itemTotal = item != null && quantity > 0 ? item.Price * quantity : 0;

                return Json(new
                {
                    success = true,
                    cartCount = cart.Sum(i => i.Quantity),
                    cartTotal = cartTotal.ToString("C"),
                    itemTotal = itemTotal.ToString("C")
                });
            }

            return Json(new { success = false, message = "Товар не найден" });
        }

        // AJAX: удаление из корзины
        [HttpPost]
        public IActionResult RemoveFromCartAjax(int productId)
        {
            var cart = GetCart();
            var item = cart.FirstOrDefault(i => i.ProductId == productId);

            if (item != null)
            {
                cart.Remove(item);
                SaveCart(cart);

                var cartTotal = cart.Sum(i => i.Price * i.Quantity);

                return Json(new
                {
                    success = true,
                    cartCount = cart.Sum(i => i.Quantity),
                    cartTotal = cartTotal.ToString("C")
                });
            }

            return Json(new { success = false, message = "Товар не найден" });
        }
    }
}