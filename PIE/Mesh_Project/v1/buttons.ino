// ---------------- Button press ----------------

int buttonState = 0;
int lastButtonState = 0;

// ----------------------------------------------

void setupButtons()
{
    pinMode(BUTTON_PIN, INPUT);

}

void handdleButtonPress()
{
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState)
    {
        if (buttonState == HIGH)
        {
            changeLEDPattern();
            sendMessage("switch light mode");
        }
        lastButtonState = buttonState;
        // delay(30);
    }
}
