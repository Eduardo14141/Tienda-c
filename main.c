#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maximum_prods 20

/*
 * Remembering:
 * unsigned int: non-negative values (in contrast "int" allows negative values as -5)
 * stdin: pointer to STDIN (standard input) stream
 * perror: function to print on STDERR (standard error) stream
 * tokenize: split the string by tokens. Example:
 *      "Hi, welcome to our store"
 *      Tokenize by commas
 *      ["Hi", "welcome to our store"]
 * \t: stands to tab character
 * Formats:
 *      - %.2f: Float with 2 decimal precision
 *      - %zu: unsigned integer
 */

/**
 * struct containing the product information
 */
typedef struct Product {
	unsigned long id;
	char *name;
	float price;
} Product;

/**
 * struct containing the information about what
 * product is purchased and in what quantity
 */
typedef struct TicketDetail {
	Product product;
	unsigned int quantity;
} TicketDetail;

/**
 * Pointer to the file
 */
FILE *price_list;
/**
 * File path were the prices list is stored
 */
char *filepath = "/home/lalo/Documentos/JetBrainsProjects/CLionProjects/Tiendita/prices.tsv";
/*
 * if and only if prices.tsv is in the same dir as main.exe
 * char *filepath = "prices.tsv";
 */
//char *filepath = "C://Users/Jazmin/Documentos/Programación/Tiendita/prices.tsv";

/**
 * Our file is delimited by tabs (Tabular separated valued (TSV))
 * we use this to tokenize the read string from the file
 */
char *file_delimiter = "\t";

// They're used to clean STDIN
/**
 * Pointer to NULL, it'll be used on getline func
 * to clean the stdin input
 */
char *pnull = NULL;
size_t line_size = 0;

/**
 * This array stores all the products available
 * in the store
 */
Product products[maximum_prods];
/**
 * This array acts as a cart, it stores which products
 * the customer wants to buy and in which quantities
 */
TicketDetail ticket[maximum_prods];

void readList();
void printMenu();
void readUnsignedLongNumber(unsigned long*, unsigned short);
void startSales();
void addToCart();
void removeFromCart();
void printFinalSale();

int main() {
	readList();
	startSales();
	return EXIT_SUCCESS;
}

/**
 * This function reads the products and save them into
 * products array
 */
void readList() {
	// open the file in read mode
	price_list = fopen(filepath, "r");
	// if file can't be accessed
	if (price_list == NULL) {
		// prints an error message
		perror("No se pudo leer el archivo de precios, por favor revisa que exista el archivo y tengas acceso");
		// forces the program to end with status code EXIT_FAILURE
		exit(EXIT_FAILURE);
	}

	// example from https://linux.die.net/man/3/getline
	char *line = NULL;
	size_t len = 0;
	char *temp;

	// parsing TSV line by line
	// if a line is successfully read && my array has space
	// getline(&line, &len, price_list) != -1 && i < maximum_prods
	for (int i = 0; getline(&line, &len, price_list) != -1 && i < maximum_prods; ++i) {
		// we tokenize the read line
		// First we pass the string chain
		temp = strtok(line, file_delimiter);
		// Defining a new product
		Product p;
		p.id = strtol(temp, NULL, 10);
		// then we pass NULL pointer
		temp = strtok(NULL, file_delimiter);
		p.name = strdup(temp);
		temp = strtok(NULL, file_delimiter);
		p.price = strtof(temp, NULL);
		// saving this product in the array
		products[i] = p;
	}
}

/**
 * Prints the price list
 */
void printMenu(){
	printf("%s", "****************************************"
	       "\n\tTienda de abarrotes \"La binaria\"\n"
	       "****************************************\n"
		   "ID\tProducto\tPrecio\n");
	for (int i = 0; products[i].id !=0; ++i) {
		printf("%zu\t%s\t$%.2f\n", products[i].id, products[i].name, products[i].price);
	}
	printf("%s", "****************************************\n");
}

/**
 * This function reads a number from stdin as string and
 * tries to convert it to a unsigned long long int value
 * if negative number is read, then sets the value to 0
 * @param number The pointer to the variable where the result will be written
 * @param radix The input number radix must be between 2 and 36 inclusive
 */
void readUnsignedLongNumber(unsigned long * number, unsigned short radix) {
	char temp[20];
	fgets(temp, 20, stdin);
	// If the string contains a '-' then sets the value to 0
	if (strchr(temp, '-') != NULL)
		*number = 0;
	else
		*number = strtoul(temp, NULL, radix);
}

/**
 * This function ask the user what product he or she is
 * willing to buy, then it checks the product actually exists
 * and register the order
 */
void addToCart() {
	unsigned long p = 0, q = 0;
	printf("Seleccione el producto a comprar:\n");
	readUnsignedLongNumber(&p, 10);
	printf("Seleccione la cantidad a comprar:\n");
	readUnsignedLongNumber(&q, 10);
	// checks the product actually exists
	if (p > maximum_prods || p == 0 || products[--p].id == 0) {
		printf("%s", "El producto no existe, intente otra vez\n");
		return;
	}
	if (q == 0) {
		printf("%s", "La cantidad parece no ser adecuada, intente otra vez\n");
		return;
	}
	ticket[p].quantity += q;
	ticket[p].product = products[p];
}

/**
 * Removes a product from the cart (cancels the order)
 */
void removeFromCart() {
	unsigned long p = 0;
	printf("¿Qué producto desea quitar?:\n");
	readUnsignedLongNumber(&p, 10);
	// checks the product actually exists
	if (p > maximum_prods || p == 0 || products[--p].id == 0) {
		printf("%s", "El producto no existe, intente otra vez\n");
		return;
	}
	// It's enough to set quantity of that product to 0
	ticket[p].quantity = 0;
}

/**
 * Prints the ticket, for that, it only prints the TicketDetail
 * items whose quantity is greater than zero
 */
void printFinalSale() {
	float total = 0, price;
	unsigned long q;
	printf("%s", "****************************************\n"
	             "Cuenta a pagar\n"
	             "****************************************\n");
	printf("%s", "Producto\tPrecio\tCantidad\tSubtotal\n");

	for (int i = 0; i < maximum_prods; ++i) {
		// skipping non bought items
		if (ticket[i].quantity == 0)
			continue;
		price = ticket[i].product.price;
		q = ticket[i].quantity;
		// printing ticket information
		printf("%s\t%.2f\t%zu\t%.2f\n", ticket[i].product.name, price, q, price * q);
		// adding subtotal
		total += price * q;
	}
	printf("%s:\t%.2f", "Total a pagar", total);
	exit(EXIT_SUCCESS);
}

/**
 * Loop asking the user what action he or she wants to do
 * and executing a function on response
 */
void startSales() {
	printMenu();
	printf("%s", "Seleccione una opción (Digite el número)\n");
	while (1) {
		printf(
				"1.- Añadir algo a la cesta\n"
			   "2.- Borrar producto\n"
			   "3.- Imprimir nuevamente la lista de precios\n"
			   "4.- Pagar y salir\nOpción: "
		);
		// reading the selected option
		int option = getchar();
		getline(&pnull, &line_size, stdin);
		// No parsing is needed
		switch (option) {
			case '1':
				addToCart();
				break;
			case '2':
				removeFromCart();
				break;
			case '3':
				printMenu();
				break;
			case '4':
				printFinalSale();
				return;
			default:
				printf("%s", "Por favor introduzca un número del 1 al 4\n");
				break;
		}
	}
}