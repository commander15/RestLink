-- Insert Products
INSERT INTO Products (name, description, price, barcode) VALUES
('Apple', 'Fresh red apple', 0.50, '1234567890123'),
('Banana', 'Organic banana', 0.30, '2234567890123'),
('Milk', '1L whole milk', 1.20, '3234567890123');

-- Insert Stocks
INSERT INTO Stocks (quantity, product_id) VALUES
(100, 1),
(150, 2),
(80, 3);

-- Insert Sales
INSERT INTO Sales (number, amount) VALUES
(1, 3.00),
(2, 1.80);

-- Insert SaleItems
INSERT INTO SaleItems (quantity, sale_id, product_id) VALUES
(2, 1, 1),
(1, 1, 3),
(3, 2, 2);

