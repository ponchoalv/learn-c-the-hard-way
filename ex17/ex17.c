#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address
{
    int id;
    int set;
    char *name;
    char *email;
};

struct Database
{
    int max_data;
    int max_rows;
    struct Address **rows;
};

struct Connection
{
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn)
{
    if (conn)
    {
        if (conn->file)
        {
            fclose(conn->file);
        }

        if (conn->db)
        {
            if (conn->db->rows)
            {
                for (size_t i = 0; i < conn->db->max_rows; i++)
                {
                    struct Address *addr = conn->db->rows[i];
                    if (addr->email)
                        free(addr->email);
                    if (addr->name)
                        free(addr->name);
                    free(addr);
                }
                free(conn->db->rows);
            }
            free(conn->db);
        }
        free(conn);
    }
}

void die(struct Connection *conn, const char *message)
{
    if (errno)
    {
        perror(message);
    }
    else
    {
        printf("ERROR: %s\n", message);
    }

    Database_close(conn);
    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    // read the two int values representing the max_data and max_rows of the database
    int rc = fread(&conn->db->max_data, sizeof(int), 1, conn->file);
    if (rc != 1)
        die(conn, "Failed to load database max_data.");

    rc = fread(&conn->db->max_rows, sizeof(int), 1, conn->file);
    if (rc != 1)
        die(conn, "Failed to load database max_rows.");

    conn->db->rows = (struct Address **)malloc(sizeof(struct Address *) * conn->db->max_rows);
    if (conn->db->rows == NULL)
        die(conn, "Cannot address memory for DB rows.");

    for (int i = 0; i < conn->db->max_rows; i++)
    {
        conn->db->rows[i] = (struct Address *)malloc(sizeof(struct Address));
        struct Address *row = conn->db->rows[i];
        if (fread(&row->id, sizeof(int), 1, conn->file) != 1)
            die(conn, "Database load : Could not read Address::id");
        if (fread(&row->set, sizeof(int), 1, conn->file) != 1)
            die(conn, "Database load : Couldn't read  Address::set");

        row->name = malloc(sizeof(char) * conn->db->max_data);
        if (row->name == NULL)
            die(conn, "Failed to address memory for the email");
        rc = fread(row->name, sizeof(char) * conn->db->max_data, 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address email.");

        row->email = malloc(sizeof(char) * conn->db->max_data);
        if (row->email == NULL)
            die(conn, "Failed to address memory for the name");
        rc = fread(row->email, sizeof(char) * conn->db->max_data, 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address name.");
    }
}

struct Connection *Database_open(const char *filename, char mode)
{
    struct Connection *conn = malloc(sizeof(struct Connection));

    if (!conn)
    {
        die(conn, "Memory error.");
    }

    conn->db = malloc(sizeof(struct Database));

    if (!conn->db)
    {
        die(conn, "Memory error.");
    }

    if (mode == 'c')
    {
        conn->file = fopen(filename, "w");
    }
    else
    {
        conn->file = fopen(filename, "r+");
        if (conn->file)
        {
            Database_load(conn);
        }
    }
    if (!conn->file)
    {
        die(conn, "Failed to open the file");
    }

    return conn;
}

void Database_write(struct Connection *conn)
{
    rewind(conn->file);

    int rc = fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);
    if (rc != 1)
    {
        die(conn, "Failed to write database.");
    }

    rc = fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);
    if (rc != 1)
    {
        die(conn, "Failed to write database.");
    }

    for (int i = 0; i < conn->db->max_rows; i++)
    {
        struct Address *row = conn->db->rows[i];
        rc = fwrite(&row->id, sizeof(int), 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address row id");

        rc = fwrite(&row->set, sizeof(int), 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address row set");

        rc = fwrite(row->name, sizeof(char) * conn->db->max_data, 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address name.");

        rc = fwrite(row->email, sizeof(char) * conn->db->max_data, 1, conn->file);
        if (rc != 1)
            die(conn, "Failed to write database address email.");
    }

    rc = fflush(conn->file);
    if (rc == 1)
        die(conn, "Cannot flush database.");
}

void Database_create(struct Connection *conn, int max_data, int max_rows)
{
    conn->db->max_data = max_data;
    conn->db->max_rows = max_rows;
    conn->db->rows = (struct Address **)malloc(sizeof(struct Address *) * conn->db->max_rows);

    for (int i = 0; i < conn->db->max_rows; i++)
    {
        conn->db->rows[i] = (struct Address *)malloc(sizeof(struct Address));
        conn->db->rows[i]->id = i;
        conn->db->rows[i]->set = 0;
        conn->db->rows[i]->name = (char *)malloc(sizeof(char) * conn->db->max_data);
        // conn->db->rows[i]->name = (char *)memset(conn->db->rows[i]->name, ' ', conn->db->max_data);
        conn->db->rows[i]->email = (char *)malloc(sizeof(char) * conn->db->max_data);
        // conn->db->rows[i]->email = (char *)memset(conn->db->rows[i]->email, ' ', conn->db->max_data);
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    struct Address *addr = conn->db->rows[id];
    if (addr->set)
        die(conn, "Already set, delete it first.");

    addr->set = 1;

    addr->name = malloc(sizeof(char) * conn->db->max_data);
    addr->email = malloc(sizeof(char) * conn->db->max_data);

    // WARNING: bug, read the "How to break it" and fix it
    char *res = strncpy(addr->name, name, conn->db->max_data);
    // demonstrate the strncpy bug
    if (!res)
        die(conn, "name copy failed.");
    addr->name[conn->db->max_data - 1] = '\0';

    res = strncpy(addr->email, email, conn->db->max_data);
    if (!res)
        die(conn, "email copy failed.");
    addr->email[conn->db->max_data - 1] = '\0';
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = conn->db->rows[id];
    if (addr->set)
    {
        Address_print(addr);
    }
    else
    {
        die(conn, "ID is not set.");
    }
}

void Database_delete(struct Connection *conn, int id)
{
    conn->db->rows[id]->set = 0;
}

void Database_list(struct Connection *conn)
{
    struct Database *db = conn->db;
    for (int i = 0; i < conn->db->max_rows; i++)
    {
        struct Address *cur = db->rows[i];
        if (cur->set)
            Address_print(cur);
    }
}

int id_get(struct Connection *conn, const char *argv[])
{
    int id = atoi(argv[3]);
    if (id >= conn->db->max_rows)
        die(conn, "There's not that many records.");
    return id;
}

int main(int argc, const char *argv[])
{
    if (argc < 3)
        die(NULL, "USAGE: ex17 <dbfile> <action> [action params]");

    const char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *conn = Database_open(filename, action);

    switch (action)
    {
    case 'c':
        if (argc != 5)
            die(conn, "Need database filename MAX_DATA and MAX_ROWS");

        Database_create(conn, atoi(argv[3]), atoi(argv[4]));
        Database_write(conn);
        break;
    case 'g':
        if (argc != 4)
            die(conn, "Need an id to get.");

        Database_get(conn, id_get(conn, argv));
        break;
    case 's':
        if (argc != 6)
            die(conn, "Need id, name, email to set.");

        Database_set(conn, id_get(conn, argv), argv[4], argv[5]);
        Database_write(conn);
        break;
    case 'd':
        if (argc != 4)
        {
            die(conn, "Need id to delete.");
        }

        Database_delete(conn, id_get(conn, argv));
        Database_write(conn);
        break;
    case 'l':
        Database_list(conn);
        break;

    default:
        die(conn, "Invalid action: c=create, g=get, s=set, d=del, l=list");
        break;
    }

    Database_close(conn);

    return 0;
}
