#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

void webserver_init(void);
#define BAUDRATE_STRING "<option value=\"115200\" %s>115200</option>\
												<option value=\"38400\" %s>38400</option>\
												<option value=\"19200\" %s>19200</option>\
												<option value=\"9600\" %s>9600</option>"
												
#define	DATABITS_STRING "<option value=\"5\" %s>5</option>\
              <option value=\"6\" %s>6</option>\
              <option value=\"7\" %s>7</option>\
              <option value=\"8\" %s>8</option>"

#define PARITY_STRING "<option value=\"0\" %s>None</option>\
              <option value=\"1\" %s>Odd</option>\
              <option value=\"2\" %s>Even</option>"

#define STOPBIT_STRING  "<option value=\"1\" %s>1</option>\
												<option value=\"2\" %s>2</option>"

#define IPADDR_STRING "<td align=\"right\"><input style=\"text-align:center;\" maxlength=\"16\" size=\"16\" name=\"ipaddress\" value=\"%s\"></td>"
#define GATEWAY_STRING "<td align=\"right\"><input style=\"text-align:center;\" maxlength=\"16\" size=\"16\" name=\"gateway\" value=\"%s\"></td>"
#define NETMASK_STRING "<td align=\"right\"><input style=\"text-align:center;\" maxlength=\"16\" size=\"16\" name=\"netmask\" value=\"%s\"></td>"
#endif

