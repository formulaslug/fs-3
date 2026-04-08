#include "mbed.h"

static BufferedSerial pc(USBTX, USBRX); // tx, rx for nucleo L432KC
static EventQueue event_queue(32 * EVENTS_EVENT_SIZE);


static DigitalOut led(LED1);
char command_accumulator[64];
int accum_idx = 0;
std::string dingdong = "ding";

FileHandle *mbed::mbed_override_console(int fd)
{
    return &pc;
}


enum Command {
    bingus,
	regen_profile,
	status_rtd,
	LED_ON
};

void process_command (char* buf, int num) {
    std::string sBuf(buf);
    
    std::size_t space_pos = sBuf.find(' ');
    std::string command = sBuf.substr(0, space_pos); 
    std::string args = (space_pos != std::string::npos) ? sBuf.substr(space_pos + 1) : ""; 
    
    printf("\r\n[DEBUG EXECUTE] Command: '%s' | Args: '%s'\r\n", command.c_str(), args.c_str());
	// how to execute command? I might need to use a combination of switch and ifelse first for the string and then for the cmd.
    // ai says can use mbed::callback function, more research needed
	//printf(buf);
}

void drain_serial_buffer()
{
    char c;
    // Read all available characters currently in the BufferedSerial
    while (pc.readable()) {
        ssize_t bytes_read = pc.read(&c, 1); 
        
        if (bytes_read == 1) { 
			pc.write(&c, 1); // Echo the character back to the terminal so its not just lost to the void, helps to see what im typing
            
			if (c == '\n' || c == '\r') { // if new line or return, process
				//printf("End of command received: %.*s\n", accum_idx, command_accumulator);
				command_accumulator[accum_idx] = '\0'; // make sure it's null terminated for processing
                if (accum_idx > 0) { 
                    process_command(command_accumulator, accum_idx); 
                    accum_idx = 0; // Reset buffer for the next command
                }

				printf("\r\n> "); // displays a fresh terminal chungus
                fflush(stdout);
            }
            else if (accum_idx < sizeof(command_accumulator) - 1) {
                command_accumulator[accum_idx++] = c;
            }
        }
    }
}

void on_rx_interrupt()
{
	// Schedule the buffer draining and command processing in the event queue
	event_queue.call(drain_serial_buffer);
}

void ding_dong_song()
{
	static int counter = 0;
    dingdong = (counter % 2 == 0) ? "ding" : "dong";
	// save cursor position ( [s ), move to top left ( [1;1H ), clear line ( [K ), print counter, move cursor back to saved position ( [u )
	printf("\033[s\033[1;1H\033[K%s: %d\033[u", dingdong.c_str(), counter++);
    fflush(stdout);
}


int main() 
{
	// \033[2J clears the screen, \033[H moves the cursor to the top-left corner
	printf("\033[2J\033[H");
    printf("\n\n> ");
    fflush(stdout); 

	pc.set_baud(115200);
	pc.set_blocking(false);
	pc.sigio(callback(on_rx_interrupt)); // register the function to be called when interrupt occurs
	

	event_queue.call_every(100ms, ding_dong_song);
	event_queue.dispatch_forever();

	return 0;
}