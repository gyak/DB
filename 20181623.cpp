#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "mysql.h"
#include <algorithm>

using namespace std;
#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "k7812453";
const char* db = "new_schema";
MYSQL* connection = NULL;
MYSQL conn;
MYSQL_RES* sql_result;
MYSQL_ROW sql_row;

typedef struct Intint {
	int x;
	int y;
}intint;

#define MAXQUERY 10000
void querying();
int makeSchema();
void deleteSchema();
bool compare1(intint, intint);


int main() {
	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}
	else
	{
		printf("Connection Succeed\n");
		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}
		int state = makeSchema();
		querying();
		mysql_close(connection);
	}
	return 0;
}

int makeSchema()
{
	char queryTemp[MAXQUERY];
	FILE* fp = fopen("test.txt", "r");
	int state = 0;
	fgets(queryTemp, MAXQUERY, fp);
	queryTemp[strlen(queryTemp) - 1] = NULL;//enter ����
	while (!feof(fp)) {
		state = mysql_query(connection, queryTemp);
		memset(queryTemp, 0, sizeof(queryTemp));
		fgets(queryTemp, MAXQUERY, fp);
		if (feof(fp) == 1)
			break;
		queryTemp[strlen(queryTemp) - 1] = NULL;
	}
	fclose(fp);
	return state;
}

void querying()
{
	int choose, state, rowNum;
	char numTemp[10], idTemp[10], phoneTemp[20];
	char queryTemp[MAXQUERY];
	while (1) {
		printf("\n\n\n\n------- SELECT QUERY TYPES -------\n\n");
		printf("\t1. TYPE 1\n");
		printf("\t2. TYPE 2\n");
		printf("\t3. TYPE 3\n");
		printf("\t4. TYPE 4\n");
		printf("\t5. TYPE 5\n");
		printf("\t6. TYPE 6\n");
		printf("\t7. TYPE 7\n");
		printf("\t0. QUIT\n");
		printf("choose number: ");
		scanf("%d", &choose);
		if (choose == 1) {
			printf("\n\n------- TYPE 1 -------\n\n");
			printf("press tracking number(800001~800021): ");
			scanf("%s", numTemp);
			if(atoi(numTemp) < 800001 || atoi(numTemp) > 800021) {
				printf("incorrect tracking number!\n");
				continue;
			}
			numTemp[strlen(numTemp) + 1] = '\0';//800001\0 = > 800001\0\0
			numTemp[strlen(numTemp)] = '\'';//800001\0\0 => 800001'\0
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select customer_id from delivery as D where D.delivery_num = '");
			strcat(queryTemp, numTemp);
			state = mysql_query(connection, queryTemp);
			//�Է��� ��� ��ȣ�� �ش��ϴ� �� id ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				sql_row = mysql_fetch_row(sql_result);
				memset(idTemp, 0, sizeof(idTemp));
				strcpy(idTemp, sql_row[0]);
				strcat(idTemp, "'");
				mysql_free_result(sql_result);
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select phone_num from customerinfo as D where D.customer_id = '");
				strcat(queryTemp, idTemp);
				state = mysql_query(connection, queryTemp);
				//�ش� id�� �´� ���� �޴��� ��ȣ�� ������
				if (state == 0) {
					sql_result = mysql_store_result(connection);
					sql_row = mysql_fetch_row(sql_result);
					memset(phoneTemp, 0, sizeof(phoneTemp));
					strcpy(phoneTemp, sql_row[0]);
					mysql_free_result(sql_result);
					printf("\n\nthat customer's phone number: %s\n", phoneTemp);
				}
			}
			else {
				printf("incorrect tracking number!!\n");
				continue;
			}
			//��۹�ȣ�� ã�� ���� => �ش� �� ����ó ���
			//���� => Ȩ����
		}
		else if (choose == 2) {
			printf("\n\n------- TYPE 2 -------\n\n");
			printf("**Who is the bighand in 2021**\n\n");
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select * from (	select customer_id, sum(sum_price) as sumsum from delivery natural join (select delivery_num, sum(prod) as sum_price from(	select delivery_num, product_price * online_order_quantity as prod from product natural join onlineorder)t group by delivery_num)s where order_year = '2021' group by customer_id) as r");
			state = mysql_query(connection, queryTemp);
			//�ֹ���ȣ���� �ֹ���*��ǰ ������ ���� ���ϰ� �׿����� ��id�� ���� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				intint* xy = (intint*)malloc(rowNum * sizeof(intint));
				for (int i = 0; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					xy[i].x = atoi(sql_row[0]);
					xy[i].y = atoi(sql_row[1]);
				}//����ü�� ��id�� �۳⿡ �� ���� ���� �ְ� �迭�� ����
				sort(xy, xy + rowNum, compare1);
				mysql_free_result(sql_result);
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select person_name, phone_num from customerinfo where customerinfo.customer_id = '");
				sprintf(idTemp, "%d'", xy[0].x);
				strcat(queryTemp, idTemp);
				state = mysql_query(connection, queryTemp);
				//�� id�� �ش��ϴ� ���� ���� ������
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					sql_row = mysql_fetch_row(sql_result);
					printf("that customer's name: %s, phone: %s\n", sql_row[0], sql_row[1]);
					mysql_free_result(sql_result);
				}
				free(xy);
			}
			printf("\n\n------- Subtypes in TYPE 2 -------\n\n");
			printf("\t1. TYPE 2-1\n");
			printf("\tANOTHER. GO TO HOME\n");
			printf("choose number: ");
			scanf("%d", &choose);
			if (choose == 1) {
				printf("\n\n------- TYPE 2-1 -------\n\n");
				printf("**What is the customer's best items.**\n\n");
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select product_id, sumsum from(select customer_id, product_id, sum(online_order_quantity) as sumsum from onlineorder natural join (select delivery_num, customer_id from delivery where order_year = '2021')t group by customer_id, product_id)a where customer_id = '");
				strcat(queryTemp, idTemp);
				strcat(queryTemp, " order by sumsum DESC");
				state = mysql_query(connection, queryTemp);
				//�ش� id�� ���� ���� ���� ���� �����ߴ� ��ǰ ���̵�� �� ���� ����
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					sql_row = mysql_fetch_row(sql_result);
					strcpy(idTemp, sql_row[0]);
					mysql_free_result(sql_result);
					memset(queryTemp, 0, sizeof(queryTemp));
					strcpy(queryTemp, "select product_name from product where product_id = '");
					strcat(queryTemp, idTemp);
					strcat(queryTemp, "'");
					//�ش� id�� ���� product�� ���� ���
					state = mysql_query(connection, queryTemp);
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						sql_row = mysql_fetch_row(sql_result);
						printf("that product's name is %s\n", sql_row[0]);
						mysql_free_result(sql_result);
					}
				}
			}
		}
		else if (choose == 3) {
			printf("\n\n------- TYPE 3 -------\n\n");
			printf("**What product is sold in 2021**\n\n");
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select distinct product_name from (delivery natural join onlineorder) natural join product where order_year = '2021'");
			state = mysql_query(connection, queryTemp);
			//�۳⿡ �Ǹŵ� product_name�� ����Ʈ�� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				printf("sold list about 2021(total: %d)\n", rowNum);
				for (int i = 0; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					printf("%d. %s\n", i + 1, sql_row[0]);
				}
				mysql_free_result(sql_result);
			}
			printf("\n\n------- Subtypes in TYPE 3 -------\n\n");
			printf("\t1. TYPE 3-1\n");
			printf("\t2. TYPE 3-2\n");
			printf("\tANOTHER. GO TO HOME\n");
			printf("choose number: ");
			scanf("%d", &choose);
			int kth;
			if (choose == 1) {
				printf("\n\n------- TYPE 3-1 -------\n\n");
				printf("**What is top K items list in last 1 year.**\n\n");
				printf("K?: ");
				scanf("%d", &kth);
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select product_name, sum(product_price*online_order_quantity) as sumsum from (delivery natural join onlineorder) natural join product where order_year = '2021' group by product_id order by sumsum desc");
				state = mysql_query(connection, queryTemp);
				//���δ�Ʈ�� �۳� �Ǹűݾ� �� ����Ʈ�� ������
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					rowNum = mysql_num_rows(sql_result);
					if (kth > rowNum || kth <= 0) {
						printf("incorrect k!!\n");
						mysql_free_result(sql_result);
						continue;
					}
					printf("sold top %d list about 2021(total: %d)\n", kth, rowNum);
					for (int i = 0; i < kth; i++) {
						sql_row = mysql_fetch_row(sql_result);
						printf("%d. %s\n", i + 1, sql_row[0]);
					}
					mysql_free_result(sql_result);
				}
			}
			else if (choose == 2) {
				printf("\n\n------- TYPE 3-2 -------\n\n");
				printf("**What is the top 10%% item's list in 2021.**\n\n");
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select product_name, sum(product_price*online_order_quantity) as sumsum from (delivery natural join onlineorder) natural join product where order_year = '2021' group by product_id order by sumsum desc");
				state = mysql_query(connection, queryTemp);
				//�۳⿡ �Ǹŵ� ��ǰ�� �� �Ǹűݾ� �� ����Ʈ�� ������
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					rowNum = mysql_num_rows(sql_result);
					kth = (rowNum + 9) / 10;
					printf("sold top 10%% list about 2021(total: %d)\n", rowNum);
					for (int i = 0; i < kth; i++) {
						sql_row = mysql_fetch_row(sql_result);
						printf("%d. %s\n", i + 1, sql_row[0]);
					}
					mysql_free_result(sql_result);
				}
			}
		}
		else if (choose == 4) {
			printf("\n\n------- TYPE 4 -------\n\n");
			printf("**What product is sold many in 2021**\n\n");
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select product_name, sum(online_order_quantity) as sumsum from (delivery natural join onlineorder) natural join product group by product_id order by sumsum desc");
			state = mysql_query(connection, queryTemp);
			//�۳⿡ �Ǹŵ� product_name�� ����Ʈ�� �Ǹŷ����� �����Ͽ� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				printf("sold list about 2021(total: %d)\n", rowNum);
				for (int i = 0; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					printf("%d. %s\n", i + 1, sql_row[0]);
				}
				mysql_free_result(sql_result);
			}
			printf("\n\n------- Subtypes in TYPE 4 -------\n\n");
			printf("\t1. TYPE 4-1\n");
			printf("\t2. TYPE 4-2\n");
			printf("\tANOTHER. GO TO HOME\n");
			printf("choose number: ");
			scanf("%d", &choose);
			int kth;
			if (choose == 1) {
				printf("\n\n------- TYPE 4-1 -------\n\n");
				printf("**What is the best items list in last 1 year.**\n\n");
				printf("K?: ");
				scanf("%d", &kth);
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select product_name, sum(online_order_quantity) as sumsum from (delivery natural join onlineorder) natural join product group by product_id order by sumsum desc");
				state = mysql_query(connection, queryTemp);
				//�۳⿡ �Ǹŵ� product_name�� ����Ʈ�� �Ǹŷ����� �����Ͽ� ������
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					rowNum = mysql_num_rows(sql_result);
					if (kth > rowNum || kth <= 0) {
						printf("incorrect k!!\n");
						mysql_free_result(sql_result);
						continue;
					}
					printf("sold top %d list about 2021(total: %d)\n", kth, rowNum);
					for (int i = 0; i < kth; i++) {
						sql_row = mysql_fetch_row(sql_result);
						printf("%d. %s\n", i + 1, sql_row[0]);
					}
					mysql_free_result(sql_result);
				}
			}
			else if (choose == 2) {
				printf("\n\n------- TYPE 4-2 -------\n\n");
				printf("**What is the upper 10%% prduct_price list in 2021.**\n\n");
				memset(queryTemp, 0, sizeof(queryTemp));
				strcpy(queryTemp, "select product_name, product_price from (delivery natural join onlineorder) natural join product where order_year = '2021' group by product_id order by product_price desc");
				state = mysql_query(connection, queryTemp);
				//�۳⿡ �ȸ� ��ǰ�� �ܰ� �� ����Ʈ�� ������
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					rowNum = mysql_num_rows(sql_result);
					kth = (rowNum + 9) / 10;
					printf("sold top 10%% list about 2021(total: %d)\n", rowNum);
					for (int i = 0; i < kth; i++) {
						sql_row = mysql_fetch_row(sql_result);
						printf("%d. %s\n", i + 1, sql_row[0]);
					}
					mysql_free_result(sql_result);
				}
			}
		}
		else if (choose == 5) {
			printf("\n\n------- TYPE 5 -------\n\n");
			printf("**What product sold out in California**\n\n");

			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select product_name from product where product_id in ( select product_id from( select product_id, sum(store_quantity) as sumsum from store natural join storeinventory where store_addr like 'California_' group by store_quantity)t where sumsum = 0)");
			state = mysql_query(connection, queryTemp);
			//California�� �ִ� ��� ���忡�� ǰ���� ��ǰ�� �̸� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				printf("sold out list in California(total: %d)\n", rowNum);
				for (int i = 0; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					printf("%d. %s\n", i + 1, sql_row[0]);
				}
				mysql_free_result(sql_result);
			}
		}
		else if (choose == 6) {
			printf("\n\n------- TYPE 6 -------\n\n");
			printf("**What delivery is not arrivial on time**\n\n");
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select delivery_num, product_name from (delivery natural join onlineorder) natural join product where how_over > 0 order by delivery_num");
			state = mysql_query(connection, queryTemp);
			//�ʰ� ��۵� ��� ��ǰ�̸� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				int count = 0;
				sql_row = mysql_fetch_row(sql_result);
				printf("%d. %s", ++count, sql_row[1]);
				strcpy(idTemp, sql_row[0]);

				for (int i = 1; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					if (strcmp(idTemp, sql_row[0]) == 0) {
						printf(", %s", sql_row[1]);
					}
					else {
						printf("\n%d. %s", ++count, sql_row[1]);
						strcpy(idTemp, sql_row[0]);
					}
				}
				printf("\n");
				mysql_free_result(sql_result);
			}
		}
		else if (choose == 7) {
			printf("\n\n------- TYPE 7 -------\n\n");
			printf("**bill for May's order**\n\n");
			memset(queryTemp, 0, sizeof(queryTemp));
			strcpy(queryTemp, "select customer_id, person_name, product_name, product_price, online_order_quantity, product_price*online_order_quantity as pay from ((delivery natural join onlineorder) natural join product) natural join customerinfo where order_month = 5 order by customer_id");
			state = mysql_query(connection, queryTemp);
			//5���� �߻��� �ֹ��� ���� ���� ������
			if (state == 0)
			{
				sql_result = mysql_store_result(connection);
				rowNum = mysql_num_rows(sql_result);
				int count = 0;
				sql_row = mysql_fetch_row(sql_result);
				strcpy(idTemp, sql_row[0]);
				printf("%d. %s(%s)'s bill for May's order\n", ++count, sql_row[1], sql_row[0]);
				printf("\t%-15s, %4s quantity \t=> %8s * %-3s = %s\n", sql_row[2], sql_row[4], sql_row[3], sql_row[4], sql_row[5]);
				int hap = atoi(sql_row[5]);
				for (int i = 1; i < rowNum; i++) {
					sql_row = mysql_fetch_row(sql_result);
					if (strcmp(idTemp, sql_row[0]) == 0) {
						printf("\t%-15s, %4s quantity \t=> %8s * %-3s = %s\n", sql_row[2], sql_row[4], sql_row[3], sql_row[4], sql_row[5]);
						hap = hap + atoi(sql_row[5]);
					}
					else {
						printf("\ttotal: %d\n\n", hap);
						hap = 0;
						printf("%d. %s(%s)'s bill for May's order\n", ++count, sql_row[1], sql_row[0]);
						printf("\t%-15s, %4s quantity \t=> %8s * %-3s = %s\n", sql_row[2], sql_row[4], sql_row[3], sql_row[4], sql_row[5]);
						strcpy(idTemp, sql_row[0]);
					}
				}
				printf("\ttotal: %d\n\n", hap);
				mysql_free_result(sql_result);
			}
		}
		else if (choose == 0) {
			deleteSchema();
			return;
		}
		else
			printf("!!!!!incorrect number!!!!!!\n");
		continue;
	}
}

bool compare1(intint a, intint b)
{
	if (a.y == b.y)//���� ���� ���
		return a.x < b.x;//id�� �������� ��
	else//�Ϲ������δ�
		return a.y > b.y;//���� �۾����� ��
}

void deleteSchema()
{
	int state = 0;
	char queryTemp[MAXQUERY];
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Installment;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists OnlineOrder;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Delivery;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists DeliveryComp;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists OfflineOrder;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Package;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists PackagePrice;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists storeInventory;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists wareInventory;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists RefillStore;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists RefillWare;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists CustomerInfo;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Product;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists ManufactureComp;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Warehouse;");
	state = mysql_query(connection, queryTemp);
	memset(queryTemp, 0, sizeof(queryTemp));
	strcpy(queryTemp, "drop table if exists Store;");
	state = mysql_query(connection, queryTemp);
	return;
}