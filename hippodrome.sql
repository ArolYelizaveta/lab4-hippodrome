BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Horses" (
	"horse_id"	integer NOT NULL,
	"name"	text NOT NULL,
	"age"	integer NOT NULL,
	"experience_years"	integer NOT NULL,
	"price"	real NOT NULL,
	"owner_id"	integer NOT NULL,
	CONSTRAINT "Horses_pk" PRIMARY KEY("horse_id"),
	CONSTRAINT "Horses_Owners" FOREIGN KEY("owner_id") REFERENCES "Owners"("owner_id")
);
CREATE TABLE IF NOT EXISTS "Jockeys" (
	"jockey_id"	integer NOT NULL,
	"last_name"	text NOT NULL,
	"experience_years"	integer NOT NULL,
	"birth_year"	integer NOT NULL,
	"address"	text NOT NULL,
	CONSTRAINT "Jockeys_pk" PRIMARY KEY("jockey_id")
);
CREATE TABLE IF NOT EXISTS "Owners" (
	"owner_id"	integer NOT NULL,
	"last_name"	text NOT NULL,
	"first_name"	text NOT NULL,
	"phone"	text NOT NULL,
	"address"	text NOT NULL,
	CONSTRAINT "Owners_pk" PRIMARY KEY("owner_id")
);
CREATE TABLE IF NOT EXISTS "Races" (
	"race_id"	integer NOT NULL,
	"date"	date NOT NULL,
	"race_number"	integer NOT NULL,
	"place"	integer NOT NULL,
	"horse_id"	integer NOT NULL,
	"jockey_id"	integer NOT NULL,
	CONSTRAINT "Races_pk" PRIMARY KEY("race_id"),
	CONSTRAINT "Races_Horses" FOREIGN KEY("horse_id") REFERENCES "Horses"("horse_id"),
	CONSTRAINT "Races_Jockeys" FOREIGN KEY("jockey_id") REFERENCES "Jockeys"("jockey_id")
);
CREATE TABLE IF NOT EXISTS "Users" (
	"user_id"	INTEGER NOT NULL,
	"username"	TEXT NOT NULL UNIQUE,
	"password_hash"	TEXT NOT NULL,
	"role"	TEXT NOT NULL CHECK("role" IN ('jockey', 'owner')),
	"jockey_id"	INTEGER,
	"owner_id"	INTEGER,
	PRIMARY KEY("user_id"),
	FOREIGN KEY("jockey_id") REFERENCES "Jockeys"("jockey_id") ON DELETE CASCADE,
	FOREIGN KEY("owner_id") REFERENCES "Owners"("owner_id") ON DELETE CASCADE,
	CHECK(("role" = 'jockey' AND "jockey_id" IS NOT NULL AND "owner_id" IS NULL) OR ("role" = 'owner' AND "owner_id" IS NOT NULL AND "jockey_id" IS NULL))
);
INSERT INTO "Horses" VALUES (1,'Ветер',5,3,15000.0,1);
INSERT INTO "Horses" VALUES (2,'Гроза',4,2,12000.0,1);
INSERT INTO "Horses" VALUES (3,'Молния',6,4,18000.0,2);
INSERT INTO "Horses" VALUES (4,'Заря',7,5,22000.0,3);
INSERT INTO "Jockeys" VALUES (1,'Козлов',5,1990,'г. Минск, ул. Пушкина 2');
INSERT INTO "Jockeys" VALUES (2,'Новик',3,1995,'г. Минск, ул. Свердлова 8');
INSERT INTO "Jockeys" VALUES (3,'Мороз',10,1985,'г. Брест, ул. Московская 15');
INSERT INTO "Owners" VALUES (1,'Иванов','Иван','+375291234567','г. Минск, ул. Ленина 1');
INSERT INTO "Owners" VALUES (2,'Петрова','Елена','+375297654321','г. Брест, ул. Советская 10');
INSERT INTO "Owners" VALUES (3,'Сидоров','Алексей','+375333333333','г. Гомель, пр. Победы 5');
INSERT INTO "Races" VALUES (1,'2024-05-15',1,2,1,1);
INSERT INTO "Races" VALUES (2,'2024-05-15',1,1,2,2);
INSERT INTO "Races" VALUES (3,'2024-05-15',1,3,3,3);
INSERT INTO "Races" VALUES (4,'2024-06-20',2,1,4,1);
INSERT INTO "Races" VALUES (5,'2024-06-20',2,2,1,2);
INSERT INTO "Races" VALUES (6,'2024-06-20',2,3,2,3);
INSERT INTO "Races" VALUES (7,'2024-07-10',3,1,3,1);
INSERT INTO "Users" VALUES (1,'ivanov','hash_ivanov123','owner',NULL,1);
INSERT INTO "Users" VALUES (2,'petrova','hash_petrova456','owner',NULL,2);
INSERT INTO "Users" VALUES (3,'sidorov','hash_sidorov789','owner',NULL,3);
INSERT INTO "Users" VALUES (4,'kozlov','hash_kozlov','jockey',1,NULL);
INSERT INTO "Users" VALUES (5,'novik','hash_novik','jockey',2,NULL);
INSERT INTO "Users" VALUES (6,'moroz','hash_moroz','jockey',3,NULL);
CREATE UNIQUE INDEX IF NOT EXISTS "idx_users_jockey_id" ON "Users" (
	"jockey_id"
) WHERE "jockey_id" IS NOT NULL;
CREATE UNIQUE INDEX IF NOT EXISTS "idx_users_owner_id" ON "Users" (
	"owner_id"
) WHERE "owner_id" IS NOT NULL;
COMMIT;
