class Collection:
    def __init__(self, client, collectionName):
        if client is None:
            print("MongoDB client is null")
            return None
        self.client = client

        if '' == collectionName:
            print("MongoDB collection name is invalid")
            return None

        try:
            self.collection = self.client[collectionName]
        except errors.CollectionInvalid, e:
            print('Collection %s is not valid' % e)
            return None

    def __new__(client, collectionName):
        return

    def put(self, dictionary):
        result = self.collection.save(dictionary)
        return result

    def put(self, fileds, values):
        dictionary = dict(zip(fields, values))
        result = self.collection.save(dictionary)
        return result

    def get(self, filter_dic):
        count = 0
        cursor = self.collection.find(filter_dic)
        for document in cursor:
            print document
