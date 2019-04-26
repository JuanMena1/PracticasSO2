
class PeticionTicket {
	private:
		int id;
		int num_tickets;
	public:
		PeticionTicket(int id, int num_tickets);
		int getId();
		int getNumTickets();
};
PeticionTicket::PeticionTicket(int id, int num_tickets){
	this->id=id;
	this->num_tickets=num_tickets;
}

int PeticionTicket::getId(){
	return this->id;
}

int PeticionTicket::getNumTickets(){
	return this->num_tickets;
}