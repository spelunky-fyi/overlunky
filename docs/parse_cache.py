import os
import hashlib
import inspect
import pickle


def needs_update(files):

    BLOCK_SIZE = 65536

    def hash_files(files):
        hash = hashlib.sha256()
        for file in files:
            with open(file, "rb") as f:
                fb = f.read(BLOCK_SIZE)
                while len(fb) > 0:
                    hash.update(fb)
                    fb = f.read(BLOCK_SIZE)
        return hash.hexdigest()

    file_hash = hash_files(files)
    db_hash = ""

    if os.path.exists(".db/hash.db"):
        with open(".db/hash.db", "r") as hash_file:
            db_hash = hash_file.read()

    with open(".db/hash.db", "w") as hash_file:
        hash_file.write(file_hash)

    return db_hash != file_hash


def do_unpickle(file_path):
    if not os.path.exists(file_path):
        return None

    with open(file_path, "rb") as file:
        return pickle.Unpickler(file).load()


def do_pickle(file_path, *args):
    combined_object = {}
    for name, obj in args:
        combined_object[name] = obj

    with open(file_path, "wb") as file:
        pickle.Pickler(file).dump(combined_object)
