/* Control Routines */
void
ga_upbutton_down(void)
{
	CONTROL_STATE->upbutton_state = KEY_PRESSED;
}

void
ga_upbutton_up(void)
{
	CONTROL_STATE->upbutton_state = KEY_RELEASED;
}

void
ga_downbutton_down(void)
{
	CONTROL_STATE->downbutton_state = KEY_PRESSED;
}

void
ga_downbutton_up(void)
{
	CONTROL_STATE->downbutton_state = KEY_RELEASED;
}

void
ga_leftbutton_down(void)
{
	CONTROL_STATE->leftbutton_state = KEY_PRESSED;
}

void
ga_leftbutton_up(void)
{
	CONTROL_STATE->leftbutton_state = KEY_RELEASED;
}

void
ga_rightbutton_down(void)
{
	CONTROL_STATE->rightbutton_state = KEY_PRESSED;
}

void
ga_rightbutton_up(void)
{
	CONTROL_STATE->rightbutton_state = KEY_RELEASED;
}

void
ga_floatbutton_down(void)
{
	CONTROL_STATE->floatbutton_state = KEY_PRESSED;
}

void
ga_floatbutton_up(void)
{
	CONTROL_STATE->floatbutton_state = KEY_RELEASED;
}

void
ga_sinkbutton_down(void)
{
	CONTROL_STATE->sinkbutton_state = KEY_PRESSED;
}

void
ga_sinkbutton_up(void)
{
	CONTROL_STATE->sinkbutton_state = KEY_RELEASED;
}

void
ga_inc_freq(void)
{
	AUDIO_STATE->frequency *= STEP;
}

void
ga_dec_freq(void)
{
	AUDIO_STATE->frequency /= STEP;
}
