# Report on `customerArrival` Function Implementation

## 1. Objective
- The `customerArrival` function simulates the arrival and order processing of a customer in a coffee shop, coordinating interactions with baristas, managing waiting times, and ensuring thread safety.

## 2. Data Structures

### a. `queries` Structure
- Represents customer queries with attributes:
  - `cust_no`: Customer number
  - `coffee_type`: Type of coffee ordered
  - `t_arr`: Arrival time of the customer
  - `t_tol`: Tolerance time for waiting
  - `t_curr`: Current time during processing
  - `t_complete`: Flag indicating order completion
  - `t_coffee`: Time required to prepare the ordered coffee

### b. `coffee_maker` Structure
- Represents a coffee maker (barista) with attributes:
  - `b_no`: Barista number
  - `b_time`: Time when the barista is available
  - `cust_no`: Customer number being served
  - `coffee_type`: Type of coffee being prepared
  - `busy`: Flag indicating barista's availability

### c. `combination` Structure
- Holds arrays of customer and barista structures:
  - `cust`: Array of `queries` pointers
  - `barista`: Array of `coffee_maker` pointers

### d. `combination2` Structure
- A subset of the `combination` structure for passing arguments to threads:
  - `cust`: Pointer to a `queries` structure
  - `barista`: Array of `coffee_maker` pointers


## 3. Customer Arrival Function - Detailed Implementation Report

### 1. Input Parameters
- **`args`:** A structure (`combination2`) containing pointers to the customer (`cust`) and an array of baristas (`barista`).

### 2. Customer Arrival
- The function uses a while loop to simulate the customer waiting until their arrival time (`t_arr`). The thread sleeps for 1 second intervals until the customer's arrival time is reached.

### 3. Print Arrival Information
- A semaphore (`print_mutex`) is used to control access to the standard output, preventing race conditions during printing.
- Arrival information, including the customer number and arrival time, is printed in white color.
- The customer's order is printed in yellow color.

### 4. Wait for Previous Customer's Order
- If the customer number is greater than 1, the function waits until the previous customer's order is complete (`t_complete` is set to 1).

### 5. Handle Customer Leaving Without Order (First Customer)
- If the current customer is the first customer and they exceed their tolerance time without receiving the order, the customer leaves, and relevant information is printed in red color.
- The function then breaks out of the loop.

### 6. Barista Lock and Order Processing (First Customer)
- The function enters a loop, attempting to lock the mutex of each barista (`bari`) in sequence.
- Upon successfully locking a barista, the barista's details are updated with the customer's information, and the customer's order processing is initiated.
- The order processing includes setting the completion time, updating global variables, printing relevant information in cyan and blue colors, and releasing the mutex.

### 7. Handle Customer Leaving Without Order (Subsequent Customers)
- For customers after the first one, the function waits until the previous customer's order is complete (`t_complete` is set to 1).
- If the current customer exceeds their tolerance time without receiving the order, the customer leaves, and relevant information is printed in red color.
- The function then breaks out of the loop.

### 8. Barista Lock and Order Processing (Subsequent Customers)
- Similar to the first customer, subsequent customers enter a loop, attempt to lock barista mutexes in sequence, and process their orders when a lock is acquired.

### 9. Waiting Time Calculation
- The `wait` variable is incremented based on the waiting time for each customer, calculated as the time from arrival until order completion or customer leaving.

### 10. Wastage Tracking
- The `wasted` variable is incremented when a customer leaves without receiving their order.



### 11. Synchronization and Print Colors
- The function utilizes semaphores (`print_mutex`) to synchronize access to the standard output and prevent race conditions during printing.
- ANSI escape codes are used for color formatting in printed messages.

### 12. Time Units and Sleeps
- The function uses `sleep` to introduce delays, ensuring that the simulation progresses over time.

### 13. Considerations
- Proper synchronization is critical to avoid race conditions, especially when accessing shared variables and printing to the console.
- The function implements logic to handle different scenarios such as customers leaving without orders.

### 14. Potential Enhancements
- Refactoring the code into smaller functions for better modularity and readability.
- Adding comments for documentation and improved code understanding.
- Enhancing flexibility by allowing dynamic input for simulation parameters.



## 4. `main` Function

### a. Initialization
- Initializes a mutex (`print_mutex`) for thread-safe printing.
- Reads input values for the number of baristas (`b`), types of coffee (`k`), and number of customers (`n`).
- Allocates memory for mutexes, customer and barista structures, and related arrays.

### b. Coffee and Barista Initialization
- Reads input for coffee types, initializes baristas, and allocates memory for customer queries.

### c. Thread Creation and Joining
- Creates threads for customer arrivals using the `customerArrival` function and waits for their completion.

### d. Statistics and Output
- Prints statistics on wasted coffee and average waiting time.

## 5. Additional Notes

- Employs a combination of dynamic data structures, synchronization mechanisms, and conditional checks to simulate customer arrivals and order processing.
- Utilizes ANSI color formatting for clear and visually appealing output.
- Manages barista availability using mutex locks to avoid race conditions.
- Provides statistics on wasted coffee and average waiting time.

## 6. Conclusion
- The `customerArrival` function, along with the `main` function, effectively models the behavior of customers in a coffee shop, considering arrival times, order processing, and barista availability. The implementation demonstrates a realistic simulation with dynamic memory allocation and thread synchronization.

## Waiting Time Calculation

In the simulation, waiting time for each customer is calculated as the time a customer spends waiting for their coffee. The waiting time is determined by measuring the time difference between the customer's arrival time (`t_arr`) and the completion time of their order (`t_complete`). If a customer leaves without their order, their waiting time is considered until the point they decide to leave. The total waiting time is then accumulated for all customers.

### Average Waiting Time Calculation

The average waiting time is calculated by dividing the total waiting time by the number of customers (`n`). This metric provides an average measure of how long, on average, customers had to wait for their orders.

---

## Coffee Wastage Calculation

Coffee wastage is determined by tracking the number of customers who leave without receiving their orders (`t_complete` is not achieved before the customer decides to leave). The total number of coffees ordered (`total_coffees_ordered`) is incremented for every customer, and the count of successfully served coffees is tracked (`total_coffees_served`). The difference between these two values represents the number of wasted coffees.

### Explanation

1. **Total Coffees Ordered (`total_coffees_ordered`):**
   - This variable keeps track of the total number of coffees customers have ordered throughout the simulation.

2. **Total Coffees Served (`total_coffees_served`):**
   - This variable represents the count of customers whose orders have been successfully prepared and served.

3. **Wasted Coffees Calculation:**
   - The number of wasted coffees is calculated as the difference between the total coffees ordered and the total coffees served. It gives the count of orders that were not fulfilled, either due to customers leaving without their orders or other reasons.

### Conclusion

These metrics provide insights into the efficiency and customer satisfaction of the cafe. A lower average waiting time and minimal coffee wastage are generally indicative of a well-organized and customer-friendly service.




# Report on `Ice Cream Parlor Sim` Function Implementation


# Data Structures

## a. `machine` Structure

- Represents an ice cream machine with the following attributes:
  - `machine_id`: Identifier for the machine.
  - `tm_start`: Start time of the machine's operation.
  - `tm_end`: End time of the machine's operation.
  - `busy`: Flag indicating whether the machine is currently processing an order.

## b. `icecream` Structure

- Represents an ice cream flavor with the following attributes:
  - `name`: Name of the ice cream flavor.
  - `t_f`: Time required to prepare the ice cream flavor.

## c. `topping` Structure

- Represents a topping with the following attributes:
  - `name`: Name of the topping.
  - `q_t`: Quantity of the topping available.

## d. `queries` Structure

- Represents customer queries with the following attributes:
  - `customer_id`: Identifier for the customer.
  - `t_arr`: Arrival time of the customer.
  - `number_of_icecreams`: Number of ice creams ordered by the customer.
  - `ic_crm`: Array of ice cream flavors ordered.
  - `tops`: Array of toppings for each ice cream.
  - `top_size`: Array indicating the size of toppings for each ice cream.
  - `ice_time`: Array indicating the preparation time for each ice cream.
  - `complete`: Array indicating the completion status of each ice cream.

## e. Other Variables

- `machine *machines`: Array of machine structures.
- `icecream *icecreams`: Array of ice cream structures.
- `topping *toppings`: Array of topping structures.
- `queries *cust`: Array of customer query structures.
- `pthread_t *customer`: Array of customer thread identifiers.
- `pthread_t *machine_thread`: Array of machine thread identifiers.
- `pthread_mutex_t **queue_lock`: 2D array of mutexes for queue operations.
- `pthread_mutex_t print`: Mutex for synchronizing print operations.
- `pthread_mutex_t lock`: Mutex for general synchronization.
- `int timer = -1`: Timer variable for tracking simulation time.

## f. `Queue` Structure

- Represents a queue structure with the following attributes:
  - `front`, `rear`: Front and rear indices of the queue.
  - `size`: Current size of the queue.
  - `capacity`: Maximum capacity of the queue.
  - `array`: Array to store customer queries.

## g. Utility Function

- `int time_from_start()`: Function to calculate the time elapsed since the start of the simulation.



## 3. Function: `serve_customer`

### a. Purpose
   - Serves a customer by managing their arrival, order placement, and order processing.

### b. Input
   - Takes a pointer to a `queries` structure as an argument.

### c. Initialization
   - Casts the input argument to a `queries` structure.
   - Extracts customer ID and sets a delay based on the ID.

### d. Customer Arrival
   - Waits for the customer's arrival time.

### e. Queue Status Check
   - Checks if the queue is full.
   - If the queue is full, the customer leaves.

### f. Print Entry Details
   - Prints details when the customer enters, including their ID and order.

### g. Print Order Details
   - Prints details of each ice cream in the order along with toppings.

### h. Topping Availability Check
   - Checks if the toppings in the order exceed their quantity limits.
   - If any topping exceeds the limit, the customer leaves.

### i. Enqueue Customer
   - Adds the customer to the queue.

### j. Processing Delay
   - Introduces a delay before processing the order.

### k. Order Processing
   - Monitors machine availability and processes the order.
   - Checks if the customer was not serviced due to machine unavailability.

### l. Completion Check
   - Checks the completion status of each ice cream in the order.
   - If an ice cream is not completed, the customer leaves.

### m. Order Collection
   - If all ice creams are completed, the customer collects the order and leaves.

### n. Thread Safety
   - Uses mutex locks to ensure thread safety when accessing shared data structures.

### o. Console Output Styling
   - Prints colorful console output for better readability.

This function encapsulates the entire process of serving a customer, handling various scenarios, and providing informative console output. The use of mutex locks ensures that shared resources are accessed safely in a multi-threaded environment.




## 4. Function: `serve_machine`

### a. Purpose
- Manages the operation of an individual ice cream machine by processing customer orders.

### b. Input
- Takes a pointer to a `machine` structure as an argument.

### c. Initialization
- Casts the input argument to a `machine` structure.
- Extracts machine ID and sets a delay based on the machine's start time.

### d. Machine Activation
- Waits for the machine's start time before proceeding with order processing.
- Prints a message indicating that the machine has started working.

### e. Order Processing Loop
- Enters a continuous loop to check for incoming orders and process them.
- Checks the current time to determine when to stop processing orders.

### f. Queue Status Check
- Skips processing if the queue is empty, allowing the machine to idle.

### g. Order Processing
- Iterates through the queue to find orders that can be processed by the current machine.
- Checks if the order's arrival time has passed and if the machine is available.
- Verifies the availability of toppings and machine slots before processing an order.
- Updates topping quantities, prints messages for order processing, and marks orders as completed.

### h. Order Completion
- Removes orders from the queue upon completion of all ice creams in the order.
- Continues processing until the machine's specified end time is reached.

### i. Machine Deactivation
- Prints a message indicating that the machine has stopped working.
- Returns from the function, signaling the end of the machine's operation.

### j. Thread Safety
- Implements mutex locks to ensure thread safety when accessing shared data structures, such as the queue and topping quantities.

### k. Console Output Styling
- Enhances console output by using ANSI escape codes to introduce colors for better readability.
- Differentiates messages based on machine activity, such as start, processing, and stop messages.

### l. Error Handling
- The function is designed to handle cases where orders cannot be processed due to topping shortages or time constraints.
- Provides detailed messages when an order cannot be fulfilled.


## 4. Function: `main`

### a. Purpose
The `main` function initializes and manages the entire ice cream parlour simulation, handling user input, memory allocation, and thread creation.

### b. Input Processing
1. **Parlour Parameters:** Accepts input for the number of machines (`n`), customers (`k`), ice cream flavors (`f`), and toppings (`t`).
2. **Machine Specifications:** Reads and initializes machine details, including start and end times, and updates the global maximum time (`t_max`).
3. **Ice Cream Flavors:** Allocates memory for ice cream structures and reads flavor names and preparation times.
4. **Toppings:** Allocates memory for topping structures, reads topping names, and sets topping quantities (with -1 indicating unlimited).

### c. Customer Order Processing
1. **Dynamic Memory Allocation:** Allocates memory for the `cust` structure dynamically as orders are read. 
2. **Order Details:** Reads customer orders, populating the structure with customer ID, arrival time, number of ice creams, and associated details.
3. **Topping Details:** Dynamically allocates memory for toppings and processes each topping, capturing their names and sizes.

### d. Queue Lock Initialization
1. **Mutex Allocation:** Allocates and initializes mutex locks for each queue and ice cream combination, ensuring thread safety during queue operations.

### e. Thread Creation
1. **Thread Initialization:** Creates threads for customer and machine processing using the `pthread_create` function.
2. **Execution of Serve Functions:** Customer threads execute the `serve_customer` function, while machine threads execute the `serve_machine` function.

### f. Thread Joining
1. **Waiting for Threads:** The program waits for all customer and machine threads to complete using the `pthread_join` function, ensuring proper synchronization.

### g. Parlour Closure
1. **Simulation End:** Prints a closing message once all threads have completed their tasks, indicating the parlour is closed.

### h. Return
1. **Success Code:** Returns 0 to signify successful execution.

### i. Thread Safety and Dynamic Memory Handling
1. **Mutex Locks:** Utilizes mutex locks to ensure thread safety during shared resource access.
2. **Dynamic Memory Allocation:** Dynamically allocates and reallocates memory for variable-size inputs, adapting to different order complexities without size limitations.

## 1. Minimizing Incomplete Orders

### Approach
To minimize incomplete orders and uphold the parlor's reputation, the simulation can be enhanced with the following strategies:

1. **Dynamic Topping Quantity Adjustments:**
   - Implement a dynamic adjustment of topping quantities based on demand and availability.
   - Regularly monitor topping usage and adjust quantities to ensure sufficient stock for popular choices.

2. **Real-time Topping Availability Checks:**
   - Conduct real-time checks for topping availability before accepting an order.
   - If any topping is insufficient, reject the order instantly, preventing incomplete orders.

3. **Order Queue Prioritization:**
   - Prioritize processing orders with toppings that have ample quantities, minimizing the chance of rejection.
   - Implement a smart queue management system to optimize order processing based on available ingredients.

## 2. Ingredient Replenishment

### Approach
To handle ingredient replenishment and ensure a continuous supply, the simulation can incorporate the following features:

1. **Automated Supplier Interaction:**
   - Integrate an automated system to contact the nearest supplier when ingredient quantities fall below a predefined threshold.
   - Implement communication mechanisms (APIs, messaging) to request topping replenishments in real-time.

2. **Dynamic Order Acceptance:**
   - Adjust the order acceptance process based on ingredient availability.
   - Temporarily stop accepting orders if crucial ingredients are critically low, preventing unnecessary commitments.

3. **Dynamic Topping Replenishment:**
   - Simulate the delivery of toppings in the simulation, dynamically updating quantities upon replenishment.
   - Consider delivery times and stock-up quantities based on historical demand patterns.

## 3. Avoiding/Minimizing Unserviced Orders

### Approach
To avoid or minimize the number of unserviced orders and enhance customer satisfaction, the simulation can incorporate the following strategies:

1. **Realistic Time Estimations:**
   - Improve time estimations for ice cream preparation, considering machine processing times and concurrent order handling.
   - Ensure customers receive accurate estimates of when their orders will be ready.

2. **Dynamic Machine Availability:**
   - Implement a dynamic machine availability model, adjusting machine work schedules based on demand patterns.
   - Optimize machine usage to minimize idle times and increase overall order processing efficiency.

3. **Order Reservations:**
   - Allow customers to reserve slots during peak hours to manage demand.
   - Provide an option for customers to pre-order, ensuring timely service and reducing walk-in wait times.

4. **Parallel Order Processing:**
   - Optimize thread management to handle multiple orders concurrently, reducing overall order processing times.
   - Implement efficient algorithms to maximize machine and staff utilization.




