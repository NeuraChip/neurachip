# Reset MongoDB Database

# Ask user if they want to reset the database
print('\n\n')
print('******************************************************************')
print('WARNING: You are about to reset the NeuraChip MongoDB database')
print('This will delete all data stored in the NeuraChip MongoDB database')
print('This action cannot be undone')
print('******************************************************************')
print('')

reset = input('Reset NeuraChip MongoDB? (y/n): ')
if reset != 'y':
	print('NeuraChip MongoDB reset aborted')
	exit()

print('\n')
# Import mongoDB
from pymongo import MongoClient
# Connect to mongoDB
client = MongoClient('localhost', 27017)
# Drop all databases except UI_DB, admin, config, local
for db in client.list_database_names():
	if db != 'UI_DB' and db != 'admin' and db != 'config' and db != 'local':
		print('Dropping database: ' + db)
		client.drop_database(db)

# Get UI_DB from mongoDB
db = client['UI_DB']
collection = db['UI_COLLECTION']
# Delete all documents in UI_COLLECTION
collection.delete_many({})
print('\n')
print('NeuraChip MongoDB reset complete')
print('\n')



