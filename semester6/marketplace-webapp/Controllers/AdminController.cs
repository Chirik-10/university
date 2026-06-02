#nullable disable
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using InternetShop.Data;
using InternetShop.Models;

namespace InternetShop.Controllers
{
    public class AdminController : Controller
    {
        private readonly ApplicationDbContext _context;
        private readonly IWebHostEnvironment _webHostEnvironment;

        public AdminController(ApplicationDbContext context, IWebHostEnvironment webHostEnvironment)
        {
            _context = context;
            _webHostEnvironment = webHostEnvironment;
        }

        private bool IsAdmin()
        {
            return HttpContext.Session.GetString("UserRole") == "Admin";
        }

        public async Task<IActionResult> Products()
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");
            var products = await _context.Products.Include(p => p.Category).ToListAsync();
            return View(products);
        }

        public async Task<IActionResult> CreateProduct()
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");
            ViewBag.Categories = await _context.Categories.ToListAsync();
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> CreateProduct(IFormCollection form, IFormFile imageFile)
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");

            var product = new Product();
            product.Name = form["Name"];
            product.Description = form["Description"];
            product.Price = decimal.Parse(form["Price"]);
            product.CategoryId = int.Parse(form["CategoryId"]);
            product.Stock = int.Parse(form["Stock"]);

            if (string.IsNullOrEmpty(product.Description)) product.Description = "";

            if (imageFile != null && imageFile.Length > 0)
            {
                var extension = Path.GetExtension(imageFile.FileName).ToLower();
                var fileName = $"{Guid.NewGuid()}{extension}";
                var uploadPath = Path.Combine(_webHostEnvironment.WebRootPath, "images");
                if (!Directory.Exists(uploadPath)) Directory.CreateDirectory(uploadPath);
                var filePath = Path.Combine(uploadPath, fileName);
                using (var stream = new FileStream(filePath, FileMode.Create))
                {
                    await imageFile.CopyToAsync(stream);
                }
                product.Image = fileName;
            }
            else
            {
                product.Image = "default.png";
            }

            _context.Products.Add(product);
            await _context.SaveChangesAsync();
            TempData["Success"] = $"Товар \"{product.Name}\" успешно добавлен!";
            return RedirectToAction("Products");
        }

        public async Task<IActionResult> EditProduct(int id)
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");
            var product = await _context.Products.FindAsync(id);
            if (product == null) return NotFound();
            ViewBag.Categories = await _context.Categories.ToListAsync();
            return View(product);
        }

        [HttpPost]
        public async Task<IActionResult> EditProduct(IFormCollection form, IFormFile imageFile)
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");

            var productId = int.Parse(form["Id"]);
            var existingProduct = await _context.Products.FindAsync(productId);
            if (existingProduct == null) return NotFound();

            existingProduct.Name = form["Name"];
            existingProduct.Description = form["Description"];

            var priceStr = form["Price"].ToString().Trim().Replace(",", ".");
            existingProduct.Price = decimal.Parse(priceStr, System.Globalization.CultureInfo.InvariantCulture);
            existingProduct.CategoryId = int.Parse(form["CategoryId"]);
            existingProduct.Stock = int.Parse(form["Stock"]);

            if (imageFile != null && imageFile.Length > 0)
            {
                var extension = Path.GetExtension(imageFile.FileName).ToLower();
                var fileName = $"{Guid.NewGuid()}{extension}";
                var uploadPath = Path.Combine(_webHostEnvironment.WebRootPath, "images");
                if (!Directory.Exists(uploadPath)) Directory.CreateDirectory(uploadPath);
                var filePath = Path.Combine(uploadPath, fileName);
                using (var stream = new FileStream(filePath, FileMode.Create))
                {
                    await imageFile.CopyToAsync(stream);
                }
                existingProduct.Image = fileName;
            }

            await _context.SaveChangesAsync();
            TempData["Success"] = $"Товар \"{existingProduct.Name}\" успешно обновлен!";
            return RedirectToAction("Products");
        }

        [HttpPost]
        public async Task<IActionResult> DeleteProduct(int id)
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");

            var product = await _context.Products.FindAsync(id);
            if (product == null)
            {
                TempData["Error"] = "Товар не найден";
                return RedirectToAction("Products");
            }

            _context.Products.Remove(product);
            await _context.SaveChangesAsync();
            TempData["Success"] = $"Товар \"{product.Name}\" удален!";
            return RedirectToAction("Products");
        }

        public async Task<IActionResult> Orders()
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");
            var orders = await _context.Orders
                .Include(o => o.User)
                .Include(o => o.OrderItems)
                .ThenInclude(oi => oi.Product)
                .OrderByDescending(o => o.OrderDate)
                .ToListAsync();
            return View(orders);
        }

        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> UpdateOrderStatus(int orderId, string status, string rejectReason = "")
        {
            if (!IsAdmin()) return RedirectToAction("Login", "Account");

            var order = await _context.Orders
                .Include(o => o.OrderItems)
                .FirstOrDefaultAsync(o => o.Id == orderId);

            if (order != null)
            {
                if (status == "Confirmed")
                {
                    // Списываем товары при подтверждении
                    foreach (var item in order.OrderItems)
                    {
                        var product = await _context.Products.FindAsync(item.ProductId);
                        if (product != null)
                        {
                            product.Stock -= item.Quantity;
                        }
                    }
                    order.Status = "Confirmed";
                    TempData["Success"] = $"Заказ №{orderId} подтвержден!";
                }
                else if (status == "Cancelled")
                {
                    if (order.Status == "New")
                    {
                        // Возвращаем товары на склад
                        foreach (var item in order.OrderItems)
                        {
                            var product = await _context.Products.FindAsync(item.ProductId);
                            if (product != null)
                            {
                                product.Stock += item.Quantity;
                            }
                        }
                    }
                    order.Status = "Cancelled";
                    order.RejectReason = rejectReason;
                    TempData["Success"] = $"Заказ №{orderId} отклонен! Причина: {rejectReason}";
                }
                await _context.SaveChangesAsync();
            }
            else
            {
                TempData["Error"] = $"Заказ №{orderId} не найден";
            }

            return RedirectToAction("Orders");
        }

        // POST: /Admin/AddCategory
        [HttpPost]
        public async Task<IActionResult> AddCategory([FromBody] CategoryRequest request)
        {
            if (!IsAdmin()) return Json(new { success = false, message = "Не авторизован" });

            if (string.IsNullOrEmpty(request.Name))
            {
                return Json(new { success = false, message = "Название категории не может быть пустым" });
            }

            var existing = await _context.Categories.FirstOrDefaultAsync(c => c.Name == request.Name);
            if (existing != null)
            {
                return Json(new { success = false, message = "Категория с таким названием уже существует" });
            }

            var category = new Category { Name = request.Name };
            _context.Categories.Add(category);
            await _context.SaveChangesAsync();

            return Json(new { success = true, id = category.Id, name = category.Name });
        }

        public class CategoryRequest
        {
            public string Name { get; set; }
        }
    }
}