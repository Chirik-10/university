#nullable disable
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace InternetShop.Models
{
    public class Product
    {
        public int Id { get; set; }

        [Required(ErrorMessage = "Название товара обязательно")]
        [Display(Name = "Название")]
        public string Name { get; set; }

        // Убираем [Required] с Description
        [Display(Name = "Описание")]
        public string Description { get; set; }

        [Required(ErrorMessage = "Цена обязательна")]
        [Range(0.01, 1000000, ErrorMessage = "Цена должна быть от 0.01 до 1 000 000")]
        [Column(TypeName = "decimal(18,2)")]
        [Display(Name = "Цена")]
        public decimal Price { get; set; }

        [Required(ErrorMessage = "Выберите категорию")]
        [Display(Name = "Категория")]
        public int CategoryId { get; set; }

        public Category Category { get; set; }

        [Required(ErrorMessage = "Количество на складе обязательно")]
        [Range(0, 10000, ErrorMessage = "Количество должно быть от 0 до 10 000")]
        [Display(Name = "Количество")]
        public int Stock { get; set; }

        [Display(Name = "Изображение")]
        public string Image { get; set; } = "default.png";
    }
}