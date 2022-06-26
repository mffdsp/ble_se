#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/types.h>
#include <sys/byteorder.h>
#include <sys/printk.h>
#include <zephyr.h>
#include <kernel.h>
#include <services/application.h>
#include <services/peripheral.h>

static void on_ble_rx_data(const uint8_t *buffer, size_t len) {
    printk("Executing ble on rx data \n");
    ble_uart_service_transmit(buffer, len);
    printk("Finished \n");
}

static void on_ble_stack_ready(struct bt_conn *conn) {
    printk("Executing ble on stack ready \n");
    (void)conn;
    ble_uart_service_register(on_ble_rx_data);
    printk(" Finished \n");
}

void main (void) {
    printk("Executing ble application start \n");
    ble_application_start(on_ble_stack_ready);
    printk("Finished \n");
}