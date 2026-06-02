#nullable disable
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using InternetShop.Data;
using InternetShop.Models;

namespace InternetShop.Controllers
{
    public class HomeController : Controller
    {
        private readonly ApplicationDbContext _context;

        public HomeController(ApplicationDbContext context)
        {
            _context = context;
        }

        public async Task<IActionResult> Index(string searchString, int categoryId = 0, decimal? minPrice = null, decimal? maxPrice = null)
        {
            var products = _context.Products.Include(p => p.Category).AsQueryable();

            if (!string.IsNullOrEmpty(searchString))
            {
                products = products.Where(p => p.Name.Contains(searchString));
                ViewBag.SearchString = searchString;
            }

            if (categoryId > 0)
            {
                products = products.Where(p => p.CategoryId == categoryId);
            }

            if (minPrice.HasValue && minPrice > 0)
            {
                products = products.Where(p => p.Price >= minPrice.Value);
            }

            if (maxPrice.HasValue && maxPrice > 0)
            {
                products = products.Where(p => p.Price <= maxPrice.Value);
            }

            ViewBag.Categories = await _context.Categories.ToListAsync();
            ViewBag.SelectedCategoryId = categoryId;
            ViewBag.MinPrice = minPrice;
            ViewBag.MaxPrice = maxPrice;

            return View(await products.ToListAsync());
        }

        // AJAX: фильтрация товаров без перезагрузки
        [HttpGet]
        public async Task<IActionResult> FilterProducts(string searchString, int categoryId = 0, decimal? minPrice = null, decimal? maxPrice = null)
        {
            var products = _context.Products.Include(p => p.Category).AsQueryable();

            if (!string.IsNullOrEmpty(searchString))
            {
                products = products.Where(p => p.Name.Contains(searchString));
            }

            if (categoryId > 0)
            {
                products = products.Where(p => p.CategoryId == categoryId);
            }

            if (minPrice.HasValue && minPrice > 0)
            {
                products = products.Where(p => p.Price >= minPrice.Value);
            }

            if (maxPrice.HasValue && maxPrice > 0)
            {
                products = products.Where(p => p.Price <= maxPrice.Value);
            }

            var result = await products.ToListAsync();
            return PartialView("_ProductsGrid", result);
        }
    }
}