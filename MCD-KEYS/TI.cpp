
static void interrupt far keypad_int_handler( void )
{

   U8 old_port_state;
   U8 new_port_state;
   U8 keyb_XMI_status;
   U8 power_XMI_status;
   U8 num_cols_pressed;

   static U16 index_poll_count = 0;
   static U16 typematic_poll_count = 0;
   static t_scan_result keypress_scan = { 0x00 };
   static t_scan_result debounce_scan = { 0x00 };
   static t_scan_result release_scan = { 0x00 };
   static t_scan_result typematic_scan = { 0x00 };
   static t_scan_result index_scan = { 0x00 };
   static t_key_timer_status key_timer_state = KEY_TIMER_OFF;
   static Bool char_index_supported = FALSE;
   static t_key_props key_previous = 
   {
      {
         CHAR_NULL,        /* key_chars[ KEY_CHAR_NUMERIC ]       */
         CHAR_NULL,        /* key_chars[ KEY_CHAR_ALPHA1 ]        */
         CHAR_NULL,        /* key_chars[ KEY_CHAR_ALPHA2 ]        */
         CHAR_NULL,        /* key_chars[ KEY_CHAR_ALPHA3 ]        */
         CHAR_NULL         /* key_chars[ KEY_CHAR_ALPHA4 ]        */
      },                   /* key_chars[ KEY_CHAR_MAX_PER_KEY ]   */
      KEY_CHAR_NUMERIC,    /* max_char_index       */
      FALSE                /* typematic_supported  */
   };

   /* This is used to determine whether or not an initial NMI was generated   */
   /* by the power button.                                                    */
   static Bool bPowerButton = FALSE;

   Bool key_valid = FALSE;
   t_col_drive col_num, col_key;
   t_row_key row_key;


  
   else if ( keyb_XMI_status & MKYPRS_WAS_XMI )
   {
      /* Source is key pressed interrupt  */
      /* Disable keyboard press from causing SMI/NMI */
      old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN );
      new_port_state = old_port_state & ~MKYPRS_WILL_XMI;
      elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN, new_port_state );

      /* Clear Key Press caused SMI/NMI interrupt flag */
      old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_STATUS  );
      new_port_state = ( old_port_state & ~MKYPRS_WAS_XMI );
      elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_STATUS, new_port_state );

      /* Indicate NMI Routine Complete */
      old_port_state = elan_read_csc( CSC_XMI_CONTROL );
      new_port_state = old_port_state | NMI_DONE;
      elan_write_csc( CSC_XMI_CONTROL, new_port_state );

      /* Perform first keyboard scan                                          */
      scan_keypad( keypress_scan );

      /* A mechanism needs to be in place that discriminates between the      */
      /* effects of noise, and genuine key presses. This will be done on a    */
      /* time basis - i.e. a key must be continually pressed for a minimum    */
      /* period of time for it to be recognised as a genuine press. This will */
      /* be determined by performing a second keyboard scan and comparing the */
      /* results from both.                                                   */
      /* The factor that limits the minimum interval between scans is the     */
      /* mechanical settling time of the switch. The maximum scan interval    */
      /* sets a limit on the key repetition rate.                             */

      /* Initiate next scan after debounce time                               */
      elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
      elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_DEBOUNCE );
      key_timer_state = KEY_TIMER_WAIT_DEBOUNCE;

      /* Enable keyboard timer to cause SMI/NMI */
      old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN );
      new_port_state = old_port_state | KYTIM_WILL_XMI;
      elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN, new_port_state );

   } /* Source is key pressed interrupt  */

   else if ( keyb_XMI_status & KYTIM_WAS_XMI )
   {

      /* Source is key timer interrupt */

      /* Disable keyboard timer from causing SMI/NMI */
      old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN );
      new_port_state = old_port_state & ~KYTIM_WILL_XMI;
      elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN, new_port_state );

      /* Clear keyboard timer caused SMI/NMI interrupt flag */
      old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_STATUS  );
      new_port_state = ( old_port_state & ~KYTIM_WAS_XMI );
      elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_STATUS, new_port_state );

      /* Actions are dependant on the current usage of the keyboard timer */
      switch ( key_timer_state )
      {
         case KEY_TIMER_WAIT_DEBOUNCE:
            /* Debounce time has elapsed, Check key validity */

            /* Was this the power button? */
            if ( bPowerButton )
            {
               /* Yes it was. */

               /* Reset the power button flag.                                         */
               bPowerButton = FALSE;

               /* Check if the button is still being pressed.                          */
               if ( elan_read_csc ( CSC_KEYB_ROWB ) & MATKB_SUS_RES )
               {
                  /* If the button is still being pressed then set the key information */
                  m_key_current = m_key_map [ 0 ] [ ROW14_KEY ];
                  write_buffer( m_key_current.key_chars[ KEY_CHAR_NUMERIC ] );
               }

               /* Setting this state re-enables interrupts and makes the keypad ready  */
               /* for use.                                                             */
               key_timer_state = KEY_TIMER_OFF;
            }

            else
            {
               scan_keypad( debounce_scan );

               /* Validation of keypress will take place in three stages         */
               /* The result of each scan must be identical                      */
               /* Only one keypress is allowed at a time. As such:  . .          */
               /* A key press must be found in one column only                   */
               /* A key press must be found in one row only                      */

               if ( ( keypress_scan[ COL3_LOW ] == debounce_scan[ COL3_LOW ] ) &&
                    ( keypress_scan[ COL4_LOW ] == debounce_scan[ COL4_LOW ] ) &&
                    ( keypress_scan[ COL7_LOW ] == debounce_scan[ COL7_LOW ] ) )
               {
                  /* Results of the scans are identical */
                  /* Check for simultaneous key presses in multiple columns */

                  num_cols_pressed = 0;

                  for ( col_num = COL3_LOW; col_num < COL_NUM_DRIVEN; col_num++ )
                  {
                     if ( keypress_scan[ col_num ] != 0x00 )
                     {
                        num_cols_pressed++;
                        col_key = col_num;
                     }
                  }

                  if ( num_cols_pressed == 1)
                  {
                     /* A key press has been found in one column only */
                     /* Check that a key press has been found in one row only */
                     key_valid = FALSE;
                     switch ( keypress_scan[ col_key ] )
                     {
                        case MATKB_ROW0:
                           row_key = ROW0_KEY;
                           key_valid = TRUE;
                           break;

                        case MATKB_ROW1:
                           row_key = ROW1_KEY;
                           key_valid = TRUE;
                           break;

                        case MATKB_ROW2:
                           row_key = ROW2_KEY;
                           key_valid = TRUE;
                           break;

                        case MATKB_ROW3:
                           row_key = ROW3_KEY;
                           key_valid = TRUE;
                           break;

                        case MATKB_ROW4:
                           row_key = ROW4_KEY;
                           key_valid = TRUE;
                           break;

                        default:
                           /* A key press has been found in no rows, */
                           /* or more than one row                   */
                           key_valid = FALSE;
                           break;
                     }

                     if ( key_valid == TRUE )
                     {
                        /* Key down Event Detected */
                        m_key_current = m_key_map [ col_key ] [ row_key ];

                        /* If we are within the character indexing 'window of */
                        /* opportunity' and another key down event occurs,    */
                        /* then the last key down event should be promoted to */
                        /* a key press, and the 'window'closed.               */ 
                        
                        if ( ( m_key_current.key_chars[KEY_CHAR_NUMERIC] != 
                               key_previous.key_chars[KEY_CHAR_NUMERIC] ) &&
                               ( char_index_supported == TRUE ) )
                        {
                           write_buffer( key_previous.key_chars[ m_key_char_idx ] );
                           char_index_supported = FALSE;
                        }

                        /* Determine whether typematic repeat should be applied  */
                        /* to this key.                                          */
                        /* Typematic repeat is supported in both alpha and       */
                        /* numeric keyboard modes, but applies to the numeric    */
                        /* character only */
                        if ( ( m_key_current.typematic_supported == TRUE ) &&
                             ( m_key_repeat_enabled != FALSE ) )
                        {
                           /* Key pressed event detected */
                           /* Generate first occurrence of this character */
                           write_buffer( m_key_current.key_chars[ KEY_CHAR_NUMERIC ] );


                           /* Initiate next keyboard scan after initial typematic delay */
                           typematic_poll_count = 0;
                           elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                           elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_TYPEM_DELAY_SCAN_PERIOD );
                           key_timer_state = KEY_TIMER_WAIT_TYPEMATIC_DELAY;
                        }
                        else
                        {   
                           /* Typematic repeat not required */
                           /* Check if Alpha Indexing is relevant to this key */
                           if ( ( m_key_mode == KEY_MODE_ALPHA ) &&
                                ( m_key_current.max_char_index > KEY_CHAR_NUMERIC ) )
                           {
                              /* Check if character index required for this press */
                              if ( ( m_key_current.key_chars[KEY_CHAR_NUMERIC] == 
                                     key_previous.key_chars[KEY_CHAR_NUMERIC] ) &&
                                   ( char_index_supported == TRUE ) )
                              {
                                 /* Character indexing required */
                                 /* Handle wrap-around here as well */
                                 m_key_char_idx++;
                                 m_key_char_idx %= m_key_current.max_char_index + 1;
                                 m_last_key_down = m_key_current.key_chars[ m_key_char_idx ];

                              }
                              else
                              {
                                 /* Generate First occurrence of this character     */
                                 /* May subsequently need alpha indexing            */
                                 /* in alphanumeric mode, make the first         */
                                 /* character generated a letter                 */
                                 m_key_char_idx = KEY_CHAR_ALPHA1;
                                 m_last_key_down = m_key_current.key_chars[ m_key_char_idx ];
                              }
                        
                           }
                           else 
                           {
                              /* Generate Only occurrence of this character */
                              write_buffer( m_key_current.key_chars[ KEY_CHAR_NUMERIC ] );
                           }

                           /* Initiate checking for key release */
                           elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                           elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_RELEASE_SCAN_PERIOD );
                           key_timer_state = KEY_TIMER_WAIT_KEY_RELEASE;
                           key_previous = m_key_current;
                        }

                     } /* if ( key_valid == TRUE ) */

                     else
                     {
                        /* A key press has been found in no rows, or more than one row */
                        /* Not a valid key press - Set up for key press interrupt      */
                        key_timer_state = KEY_TIMER_OFF;
                     }
                  }
                  else
                  {
                     /* A key press has been found in no columns, or more than one column */
                     /* Not a valid key press - Set up for key press interrupt            */
                     key_timer_state = KEY_TIMER_OFF;
                  }
               }
               else
               {
                  /* Results of the scans are different                       */
                  /* Not a valid key press - Set up for key press interrupt   */
                  key_timer_state = KEY_TIMER_OFF;
               }

            } /* if ( bPowerButton ) ... else ... */

            break; /* KEY_TIMER_WAIT_DEBOUNCE */

         case KEY_TIMER_WAIT_TYPEMATIC_DELAY:
            /* Typematic initial poll period has elapsed, check that same     */
            /* key is still pressed                                           */

            scan_keypad( typematic_scan );

            typematic_poll_count++;

            if ( ( typematic_scan[ COL3_LOW ] == keypress_scan[ COL3_LOW ] ) &&
                 ( typematic_scan[ COL4_LOW ] == keypress_scan[ COL4_LOW ] ) &&
                 ( typematic_scan[ COL7_LOW ] == keypress_scan[ COL7_LOW ] ) )
            {
               /* Results of the scans are identical  */
               /* i.e. same key is still pressed      */

               /* Prevent one too many scans */
               if ( typematic_poll_count < m_key_delay[ m_key_repeat_delay ].num_scans )
               {
                  /* Initiate next   poll */
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_TYPEM_DELAY_SCAN_PERIOD );
               }
               else
               {
                  /* Same key has been pressed continually for the typematic delay  */
                  /* Generate first repeat                                          */
                  /* Key pressed event */
                  write_buffer( m_key_current.key_chars[ KEY_CHAR_NUMERIC ] );


                  /* Initiate checking for further repeats              */
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                  elan_write_csc( CSC_KEYB_TIMER, m_key_repeat[ m_key_repeat_rate ].keyboard_timer );
                  key_timer_state = KEY_TIMER_WAIT_TYPEMATIC_PERIOD;
               }
            }
            else
            {
               /* key no longer pressed, initiate checking for key release */
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_RELEASE_SCAN_PERIOD );
               key_timer_state = KEY_TIMER_WAIT_KEY_RELEASE;
            }

            break; /* KEY_TIMER_WAIT_TYPEMATIC_DELAY */

         case KEY_TIMER_WAIT_TYPEMATIC_PERIOD:

            /* Typematic period has elapsed           */
            /* Check that same key is still pressed   */
            scan_keypad( typematic_scan );

            if ( ( typematic_scan[ COL3_LOW ] == keypress_scan[ COL3_LOW ] ) &&
                 ( typematic_scan[ COL4_LOW ] == keypress_scan[ COL4_LOW ] ) &&
                 ( typematic_scan[ COL7_LOW ] == keypress_scan[ COL7_LOW ] ) )
            {
               /* Results of the scans are identical                             */
               /* Generate third and subsequent occurrences of numeric character */
               /* key press event */
                write_buffer( m_key_current.key_chars[ KEY_CHAR_NUMERIC ] );

               /* Initiate next keyboard scan after typematic repeat period */
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
               elan_write_csc( CSC_KEYB_TIMER, m_key_repeat[ m_key_repeat_rate ].keyboard_timer );
            }
            else
            {
               /* Key no longer pressed, initiate checking for key release */
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_RELEASE_SCAN_PERIOD );
               key_timer_state = KEY_TIMER_WAIT_KEY_RELEASE;
            }
            
            break; /* KEY_TIMER_WAIT_TYPEMATIC_PERIOD */


         case KEY_TIMER_WAIT_KEY_RELEASE:
            /* Key release scan interval has elapsed. Check for key release */
            scan_keypad( release_scan );

            if ( ( release_scan[ COL3_LOW ] == 0x00 ) &&
                 ( release_scan[ COL4_LOW ] == 0x00 ) &&
                 ( release_scan[ COL7_LOW ] == 0x00 ) )
            {
               /* key up event */ 
               /* All keys released */
               /* Check for indexing of alpha characters on this key */
               if ( ( m_key_mode == KEY_MODE_ALPHA ) &&
                    ( m_key_current.max_char_index > KEY_CHAR_NUMERIC ) )
               {

                  /* For character indexing to occurr, succesive, valid       */
                  /* presses of the same key must be detected within a        */
                  /* designated time frame.                                   */

                  /* Read the WARNING in the file header regarding use of the */
                  /* keyboard timer !                                         */

                  /* Initiate first character index keyboard poll */
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_CHAR_INDEX_PERIOD );
                  key_timer_state = KEY_TIMER_CHAR_INDEX_POLL;
                  index_poll_count = 0;
               }
               else
               {
                  /* Final occurrence of this character has been generated */
                  /* Set up for key press interrupt                        */
                  key_timer_state = KEY_TIMER_OFF;
               }
            }
            else
            {
               /* A key is still pressed              */
               /* Initiate next check for key release */
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
               elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_RELEASE_SCAN_PERIOD );
            }

            break; /* KEY_TIMER_WAIT_KEY_RELEASE */

         case KEY_TIMER_CHAR_INDEX_POLL:

            /* For character indexing to occurr, the following conditions     */
            /* must be met:                                                   */
            /* The key mode must be ALPHA.                                    */
            /* The key must have be capable of generating ALPHA chracters,    */
            /* not just NUMERIC characters.                                   */
            /* Successive valid pressses of the key must occurr within a      */
            /* nominated time frame.                                          */ 
            
            index_poll_count++;

            if ( index_poll_count <= MAX_CHAR_INDEX_POLLS )
            {
               /* We are still within the character indexing 'window of       */
               /* opportunity', check for any keys down                       */
               scan_keypad( index_scan );

               if ( ( index_scan[ COL3_LOW ] != 0x00 ) ||
                    ( index_scan[ COL4_LOW ] != 0x00 ) ||
                    ( index_scan[ COL7_LOW ] != 0x00 ) )
               {
                  /* key down event, initiate debounce */
                  /* and validation process. */
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_DEBOUNCE );
                  key_timer_state = KEY_TIMER_WAIT_DEBOUNCE;
                  char_index_supported = TRUE;
               }
               else
               {
                  /* No keys down, Initiate next index poll                   */
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );
                  elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_CHAR_INDEX_PERIOD );
               }

            }
            else
            {
               /* Time frame during which presses will be considered for      */
               /* indexing purposes has expired without any key presses       */
               /* being detected.                                             */
               /* Accept the last key down as key press event                 */
               write_buffer( m_key_current.key_chars[ m_key_char_idx ] );
               char_index_supported = FALSE;
               key_timer_state = KEY_TIMER_OFF;
            }   
            
            break; /* KEY_TIMER_CHAR_INDEX_POLL */

         default:
            /* Fault condition - Set up for key press interrupt */
            key_timer_state = KEY_TIMER_OFF;
            break; /* default */

      } /* switch ( key_timer_state ) */

      /* Reset keyboard timer NMI status */
      old_port_state = elan_read_csc( CSC_KEYB_CONFIG_B );
      new_port_state = old_port_state & ~KEY_TIM_STAT_CLR;
      elan_write_csc( CSC_KEYB_CONFIG_B, new_port_state );

      /* Indicate NMI Routine Complete */
      old_port_state = elan_read_csc( CSC_XMI_CONTROL );
      new_port_state = old_port_state | NMI_DONE;
      elan_write_csc( CSC_XMI_CONTROL, new_port_state );

      if ( key_timer_state == KEY_TIMER_OFF )
      {

         /* Re-initialise for next key press */
         m_last_key_down = CHAR_NULL;
         
         /* Disable keyboard timer */
         elan_write_csc( CSC_KEYB_TIMER, KEYB_TIMER_OFF );

         /* Re-Initialise keyboard for next key press */

         /* Clear all active columns - guarantees that any key press       */
         /* causes a high to low transition on at least one row input.     */
         old_port_state = elan_read_csc( CSC_KEYB_COL );
         new_port_state = ( old_port_state & ~ACTIVE_COLS );
         elan_write_csc( CSC_KEYB_COL, new_port_state );

         /* Enable keyboard press to cause SMI/NMI */
         old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN );
         new_port_state = old_port_state | MKYPRS_WILL_XMI;
         elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN, new_port_state );

      }

      else
      {
         /* Enable keyboard timer to cause SMI/NMI */
         old_port_state = elan_read_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN );
         new_port_state = old_port_state | KYTIM_WILL_XMI;
         elan_write_csc( CSC_PC_CARD_KEYB_SMI_NMI_EN, new_port_state );
      }

   }  /* Source is key timer interrupt */

   else
   {
      /* NMI not generated by Keyboard, Call original handler */
      p_old_nmi_handler();

      #ifdef NOT_LIKELY
      /* Indicate NMI Routine Complete */
      old_port_state = elan_read_csc( CSC_XMI_CONTROL );
      new_port_state = old_port_state | NMI_DONE;
      elan_write_csc( CSC_XMI_CONTROL, new_port_state );
      #endif /* NOT_LIKELY */
   }

} /* keypad_int_handler */
