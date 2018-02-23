//static int16_t bestTwoAvg( int16_t x , int16_t y , int16_t z) {
//  int16_t da, db, dc;
//  int16_t reta = 0;
//  if ( x > y ) da = x - y; else da = y - x;
//  if ( x > z ) db = x - z; else db = z - x;
//  if ( z > y ) dc = z - y; else dc = y - z;

//  if ( da <= db && da <= dc ) reta = (x + y) >> 1;
//  else if ( db <= da && db <= dc ) reta = (x + z) >> 1;
//  else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

//  return (reta);
//}

//static bool IsThresholdViolated(uint16_t x1, uint16_t x2, uint16_t x3)
//{
//	if(x1 > x2)
//	{
//		if(x1 - x2 > THRESHOLD)
//			return false;
//		if(x1 > x3)
//		{
//			if(x1 - x3 > THRESHOLD)
//				return false;
//		}
//		else
//		{
//			if(x3 - x1 > THRESHOLD)
//				return false;
//		}
//	}
//	else
//	{
//		if(x2 - x1 > THRESHOLD)
//			return false;
//		if(x2 > x3)
//		{
//			if(x2 - x3 > THRESHOLD)
//				return false;
//		}
//		else
//		{
//			if(x3 - x2 > THRESHOLD)
//				return false;
//		}
//	}
//	return true;
//}

//bool Method1(void)
//{
//	int16_t x1, x2, x3, y1, y2, y3;
//	int16_t z1 = GetTouchLocationZ1(0);
//	int z = (int)z1 + 4095;
//	int16_t z2 = GetTouchLocationZ2(0);
//	z -= z2;
//	if (z >= Z_THRESHOLD)
//	{
//		x1 = GetTouchLocationX(0);
//		y1 = GetTouchLocationY(0);
//		x2 = GetTouchLocationX(0);
//		y2 = GetTouchLocationY(0);
//		x3 = GetTouchLocationX(0);
//		y3 = GetTouchLocationY(0);
//	}
//	else
//		return false;
////	z1 = GetTouchLocationZ1();
////	z2 = GetTouchLocationZ2();
////	if ((z2 - z1) > Z_THRESHOLD)
////		return;
//	lastTouchPos->x = bestTwoAvg(x1, x2, x3);
//	lastTouchPos->y = bestTwoAvg(y1, y2, y3);
//	return true;
//}

//bool Method2(void)
//{
//	uint16_t x1, x2, x3, y1, y2, y3;
//	x1 = GetTouchLocationX(0);
//	y1 = GetTouchLocationY(0);
//	x2 = GetTouchLocationX(0);
//	y2 = GetTouchLocationY(0);
//	x3 = GetTouchLocationX(0);
//	y3 = GetTouchLocationY(0);
//	if(IsThresholdViolated(x1, x2, x3) == false)
//		return false;
//	if(IsThresholdViolated(y1, y2, y3) == false)
//		return false;
//	
//	lastTouchPos->x = (x1 + x2 + x3) / 3;
//	lastTouchPos->y = (y1 + y2 + y3) / 3;
//	
//	return true;
//}

//bool Method3(void)
//{
//	int16_t x1, x2, y1, y2;
//	x1 = GetTouchLocationX(0);
//	y1 = GetTouchLocationY(0); 
//	uint8_t touchValDiffY = 1;
//	uint8_t touchValDiffX = touchValDiffY * 2;
//	uint16_t pressureLimit = 400;
//	bool isOk = false;
//	int retries = 40;
//	do 
//	{
//		x2 = x1;
//		y2 = y1;
//		x1 = GetTouchLocationX(0);
//		y1 = GetTouchLocationY(0);
//		uint16_t z1 = GetTouchLocationZ1(0);
//		uint16_t z2 = GetTouchLocationZ2(0);
//		
//		double p = (((z2 * 1.0) / z1) - 1);
//		
//		isOk = (p > pressureLimit) ? true : false;
//		
//		if(isOk == true)
//			isOk = (x1 == 0 || y1 == 0 || x2 == 0 || y2 == 0) ? false : true;
//		if(isOk == true)
//			isOk = x1 >= x2 ? ((x1 - x2) < touchValDiffX ? true :  false) : ((x2 - x1) < touchValDiffX ? true :  false);
//		if(isOk == true)
//			isOk = y1 >= y2 ? ((y1 - y2) < touchValDiffY ? true :  false) : ((y2 - y1) < touchValDiffY ? true :  false);
//		
//	} while((isOk == false) && (retries-- > 0));

//	if(retries <= 0)
//		return false;
//	
//	lastTouchPos->x = y1;
//	lastTouchPos->y = x1;
//	
//	return true;
//}

//bool Method4(void)
//{
//	int16_t x1, x2, y1, y2;
//	x1 = GetTouchLocationX(0);
//	y1 = GetTouchLocationY(0); 
//	uint8_t touchValDiffY = 3;
//	uint8_t touchValDiffX = touchValDiffY * 2;
//	uint16_t pressureLimit = 400;
//	bool isOk = false;
//	int retries = 40;
//	do 
//	{
//		x2 = x1;
//		y2 = y1;
//		x1 = GetTouchLocationX(0);
//		y1 = GetTouchLocationY(0);
////		uint16_t z1 = GetTouchLocationZ1();
////		uint16_t z2 = GetTouchLocationZ2();
////		
////		double p = (((z2 * 1.0) / z1) - 1) * ( 4096 - (double)x1);
////		
////		isOk = (p > pressureLimit) ? true : false;
////		
////		if(isOk == true)
//			isOk = (x1 == 0 || y1 == 0 || x2 == 0 || y2 == 0) ? false : true;
//		if(isOk == true)
//			isOk = x1 >= x2 ? ((x1 - x2) < touchValDiffX ? true :  false) : ((x2 - x1) < touchValDiffX ? true :  false);
//		if(isOk == true)
//			isOk = y1 >= y2 ? ((y1 - y2) < touchValDiffY ? true :  false) : ((y2 - y1) < touchValDiffY ? true :  false);
//		
//	} while((isOk == false) && (retries-- > 0));

//	if(retries <= 0)
//		return false;
//	
//	lastTouchPos->x = ((cal->An * x1) + (cal->Bn * y1) + cal->Cn) / cal->V;
//	lastTouchPos->y = ((cal->Dn * x1) + (cal->En * y1) + cal->Fn) / cal->V;
//	
//	return true;
//}

