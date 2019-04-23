using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClickToDisable : MonoBehaviour {
    public GameObject thePopUp;

	public void ClicktodisableGM()
    {
        thePopUp.SetActive(false);
    }
}
