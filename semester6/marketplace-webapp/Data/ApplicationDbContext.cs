using Microsoft.EntityFrameworkCore;
using InternetShop.Models;

namespace InternetShop.Data
{
    public class ApplicationDbContext : DbContext
    {
        public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options)
            : base(options)
        {
        }

        public DbSet<User> Users { get; set; }
        public DbSet<Product> Products { get; set; }
        public DbSet<Category> Categories { get; set; }
        public DbSet<Order> Orders { get; set; }
        public DbSet<OrderItem> OrderItems { get; set; }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<User>()
                .HasIndex(u => u.Email)
                .IsUnique();

            modelBuilder.Entity<Product>()
                .Property(p => p.Price)
                .HasPrecision(18, 2);

            modelBuilder.Entity<Order>()
                .Property(o => o.TotalAmount)
                .HasPrecision(18, 2);

            modelBuilder.Entity<OrderItem>()
                .Property(oi => oi.PriceAtOrder)
                .HasPrecision(18, 2);

            // Начальные данные
            modelBuilder.Entity<Category>().HasData(
                new Category { Id = 1, Name = "Электроника" },
                new Category { Id = 2, Name = "Одежда" },
                new Category { Id = 3, Name = "Книги" }
            );

            modelBuilder.Entity<Product>().HasData(
                new Product { Id = 1, Name = "Смартфон", Price = 30000m, CategoryId = 1, Stock = 10, Image = "phone.png", Description = "Современный смартфон" },
                new Product { Id = 2, Name = "Ноутбук", Price = 60000m, CategoryId = 1, Stock = 5, Image = "laptop.png", Description = "Мощный ноутбук" },
                new Product { Id = 3, Name = "Футболка", Price = 500m, CategoryId = 2, Stock = 50, Image = "tshirt.png", Description = "Хлопковая футболка" }
            );
        }
    }
}