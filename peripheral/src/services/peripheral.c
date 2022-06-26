#include <services/peripheral.h>

static ble_uart_service_rx_callback rx_callback = NULL;
#define BLE_UART_SERVICE_TX_CHAR_OFFSET    3

static struct bt_uuid_128 ble_uart_uppercase = BT_UUID_INIT_128(
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x00, 0x89, 0x67, 0x43, 0x21);

static struct bt_uuid_128 ble_uart_receive_data = BT_UUID_INIT_128(
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x01, 0x89, 0x67, 0x43, 0x21);

static struct bt_uuid_128 ble_uart_notify = BT_UUID_INIT_128(
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x02, 0x89, 0x67, 0x43, 0x21);

static struct bt_gatt_attr attrs[] = { 
        BT_GATT_PRIMARY_SERVICE(&ble_uart_uppercase),
		BT_GATT_CHARACTERISTIC(&ble_uart_receive_data, BT_GATT_CHRC_WRITE_WITHOUT_RESP,
													 BT_GATT_PERM_WRITE, NULL, client_data, NULL),
        BT_GATT_CHARACTERISTIC(&ble_uart_notify, BT_GATT_CHRC_NOTIFY,
													 BT_GATT_PERM_NONE, NULL, NULL, NULL),
        BT_GATT_CCC(ble_uart_ccc_changed, BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service peripheral = BT_GATT_SERVICE(attrs);

static ssize_t client_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags){
    (void)conn;
	(void)attr;
	(void)offset;
	(void)flags;

    if(rx_callback) {
        rx_callback((const uint8_t *)buf,len);
    }

    return len;
}

static void ble_uart_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value){
	(void)attr;
	(void)value;
}

int ble_uart_service_register(const ble_uart_service_rx_callback callback) {
    rx_callback = callback;
	return 	bt_gatt_service_register(&peripheral);
}

int ble_uart_service_transmit(const uint8_t *buffer, size_t len) {

	if(!buffer || !len) {
		return -1;
	}

    struct bt_conn *conn = ble_get_connection_ref();
    printk("%c\n",*buffer);
    if(conn) {
       return ( bt_gatt_notify(conn,
                            &attrs[BLE_UART_SERVICE_TX_CHAR_OFFSET],
                            buffer,
                            len));
    } else {
        return -1;
    }
}