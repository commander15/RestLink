CREATE TABLE Products (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        VARCHAR(30) NOT NULL,
    description VARCHAR(255),
    price       REAL NOT NULL CHECK (price >= 0),
    barcode     VARCHAR(64) UNIQUE,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE Stocks (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    quantity    INTEGER NOT NULL DEFAULT 0 CHECK (quantity >= 0),
    product_id  INTEGER NOT NULL,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (product_id) REFERENCES Products(id) ON DELETE CASCADE
);

CREATE TABLE Sales (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    number     INTEGER NOT NULL CHECK (number > 0),
    amount     REAL NOT NULL CHECK (amount >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE SaleItems (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    quantity   INTEGER NOT NULL CHECK (quantity > 0),
    sale_id    INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    FOREIGN KEY (sale_id) REFERENCES Sales(id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES Products(id)
);

