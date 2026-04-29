from sqlalchemy import create_engine, MetaData, Column, Table, String, Integer, ForeignKey, insert, select
from sqlalchemy.ext.declarative import declarative_base 
from sqlalchemy.orm import sessionmaker


engine = create_engine(
    url="sqlite:///data/airport.db",
    echo=True
)
session_local = sessionmaker(bind=engine)

base = declarative_base()

metadata_obj = MetaData()

passenger_table = Table(
    "passenger", 
    metadata_obj,
    Column("id", Integer, primary_key=True),
    Column("full_name", String(128), nullable=False),
    Column("age", Integer, nullable=False)
)

flight_table = Table(
    "flight",
    metadata_obj,
    Column("id", Integer, primary_key=True),
    Column("flight_number", String(10), nullable=False),
    Column("destination", String(64), nullable=False)
)

passenger_flight_table = Table(
    "passenger_flight",
    metadata_obj,
    Column("id", Integer, primary_key=True),
    Column("passenger_id", Integer, ForeignKey("passenger.id")),
    Column("flight_id", Integer, ForeignKey("flight.id"))
)

metadata_obj.create_all(engine)

with engine.begin() as connection:
    ins_flight = insert(flight_table).values(flight_number="PRD-4310", destination="New York")
    new_flight = connection.execute(ins_flight)
    
    ins_passnger = insert(passenger_table).values(full_name="Василий Новиков", age=13)
    new_passenger = connection.execute(ins_passnger)
    
    ins_third = insert(passenger_flight_table).values(passenger_id= new_passenger.inserted_primary_key[0], flight_id= new_flight.inserted_primary_key[0])
    
with engine.connect() as connection:
    stmt = select(passenger_table).where(passenger_table.c.age == 13)
    result = connection.execute(stmt).fetchone()
    print(f"Имя пассажира: {result.full_name}, Возраст: {result.age}")