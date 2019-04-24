
class Peticion {
	private:
		int id;
		int num_tickets;
	public:
		Peticion(int id, int num_tickets);
		int getId();
		int getNumTickets();
};
Peticion::Peticion(int id, int num_tickets){
	this->id=id;
	this->num_tickets=num_tickets;
}

int Peticion::getId(){
	return this->id;
}

int Peticion::getNumTickets(){
	return this->num_tickets;
}