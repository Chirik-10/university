#nullable disable
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using InternetShop.Data;
using InternetShop.Models;
using System.Security.Cryptography;
using System.Text;

namespace InternetShop.Controllers
{
    public class ProfileController : Controller
    {
        private readonly ApplicationDbContext _context;
        private readonly IWebHostEnvironment _webHostEnvironment;

        public ProfileController(ApplicationDbContext context, IWebHostEnvironment webHostEnvironment)
        {
            _context = context;
            _webHostEnvironment = webHostEnvironment;
        }

        public async Task<IActionResult> Index()
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return RedirectToAction("Login", "Account");

            var user = await _context.Users.FindAsync(userId);
            if (user == null) return RedirectToAction("Login", "Account");

            return View(user);
        }

        [HttpPost]
        public async Task<IActionResult> UpdateName(string newName)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return Json(new { success = false, message = "Не авторизован" });

            var user = await _context.Users.FindAsync(userId);
            if (user == null) return Json(new { success = false, message = "Пользователь не найден" });

            user.Name = newName;
            await _context.SaveChangesAsync();
            HttpContext.Session.SetString("UserName", newName);

            return Json(new { success = true, message = "Имя обновлено" });
        }

        [HttpPost]
        public async Task<IActionResult> UpdateEmail(string newEmail)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return Json(new { success = false, message = "Не авторизован" });

            var existingUser = await _context.Users.FirstOrDefaultAsync(u => u.Email == newEmail && u.Id != userId);
            if (existingUser != null) return Json(new { success = false, message = "Этот email уже используется" });

            var user = await _context.Users.FindAsync(userId);
            if (user == null) return Json(new { success = false, message = "Пользователь не найден" });

            user.Email = newEmail;
            await _context.SaveChangesAsync();

            return Json(new { success = true, message = "Email обновлён" });
        }

        [HttpPost]
        public async Task<IActionResult> UpdateAddress(string newAddress)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return Json(new { success = false, message = "Не авторизован" });

            var user = await _context.Users.FindAsync(userId);
            if (user == null) return Json(new { success = false, message = "Пользователь не найден" });

            user.Address = newAddress;
            await _context.SaveChangesAsync();

            return Json(new { success = true, message = "Адрес обновлён" });
        }

        [HttpPost]
        public async Task<IActionResult> UpdatePassword(string currentPassword, string newPassword, string confirmPassword)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return Json(new { success = false, message = "Не авторизован" });

            if (newPassword != confirmPassword) return Json(new { success = false, message = "Пароли не совпадают" });

            var user = await _context.Users.FindAsync(userId);
            if (user == null) return Json(new { success = false, message = "Пользователь не найден" });

            var hashedCurrent = HashPassword(currentPassword);
            if (user.Password != hashedCurrent) return Json(new { success = false, message = "Неверный текущий пароль" });

            user.Password = HashPassword(newPassword);
            await _context.SaveChangesAsync();

            return Json(new { success = true, message = "Пароль изменён" });
        }

        [HttpPost]
        public async Task<IActionResult> UploadAvatar(IFormFile avatar)
        {
            var userId = HttpContext.Session.GetInt32("UserId");
            if (userId == null) return Json(new { success = false, message = "Не авторизован" });

            if (avatar == null || avatar.Length == 0) return Json(new { success = false, message = "Файл не выбран" });

            var fileName = $"avatar_{userId}_{DateTime.Now.Ticks}.png";
            var uploadPath = Path.Combine(_webHostEnvironment.WebRootPath, "images", "avatars");
            if (!Directory.Exists(uploadPath)) Directory.CreateDirectory(uploadPath);

            var filePath = Path.Combine(uploadPath, fileName);
            using (var stream = new FileStream(filePath, FileMode.Create))
            {
                await avatar.CopyToAsync(stream);
            }

            var user = await _context.Users.FindAsync(userId);
            if (user != null)
            {
                user.AvatarPath = $"/images/avatars/{fileName}";
                await _context.SaveChangesAsync();
            }

            return Json(new { success = true, message = "Аватар обновлён", avatarPath = user?.AvatarPath });
        }

        private string HashPassword(string password)
        {
            using (var sha256 = SHA256.Create())
            {
                return Convert.ToBase64String(sha256.ComputeHash(Encoding.UTF8.GetBytes(password)));
            }
        }
    }
}