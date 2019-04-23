using System.Collections;

using System.Collections.Generic;

using UnityEngine;

using UnityEngine.UI;



public class AnimationMove : MonoBehaviour
{
    public enum PlayerState
    {
        Dark,
        Light,
        Dark_Red,
        Light_Red
    }
    public bool useMetric;
    public PlayerState playerState;
    private float zSpeed;
    private float xSpeed;
    private Vector3 lastPos;
    public float tiltAngle = 60.0f;

    private bool mSpacePressed = false;

    private float mTeleportTimer = 0.5f;



    private GameObject[] lightArray;
    private bool cancel = false;
    private Rigidbody rb;

    private float snapCamTimer = 0.2f;

    public bool isDark = true;

    private bool inLight = false;

    public bool isVisible = false;

    public GameObject chargeBar;

    private ParticleSystem teleportEffect;

    //public GameObject gameOverText;

    public Text charges;

    public int mCharges;

    public GameObject DarkStateText;
    public GameObject LightStateText;
    public GameObject RedStateText;

    public GameObject metricManager;

    public Image flashImage;

    private bool displayFlash = false;

    private Shader darkParticles;
    private Shader lightParticles;

    void Start()

    {
        darkParticles = Shader.Find("Particles/Multiply");
        lightParticles = Shader.Find("Particles/Additive");

        rb = GetComponent<Rigidbody>();

        chargeBar.SetActive(false);

        teleportEffect = transform.GetChild(2).gameObject.GetComponent<ParticleSystem>();

        lastPos = transform.position;

        //gameOverText.SetActive(false);
    }

    void Update()
    {
        lastPos = transform.position;
        //Vector3 velocity = new Vector3(1, 0, 0);
        //transform.position += velocity * Time.fixedDeltaTime;
        if (useMetric)
        metricManager.GetComponent<MetricManager>().AddToTime(Time.deltaTime);


        if (Input.GetKey(KeyCode.W))
        {

            zSpeed = -1;

        }
        else if (Input.GetKey(KeyCode.S))

        {

            zSpeed = 1;

        }

        else
        {

            zSpeed = 0;

        }
        if (Input.GetKey(KeyCode.A))
        {

            xSpeed = -1;

        }
        else if (Input.GetKey(KeyCode.D))

        {

            xSpeed = 1;

        }

        else
        {

            xSpeed = 0;

        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            transform.Rotate(-Vector3.up * Time.deltaTime * tiltAngle, Space.World);
        }
        else if (Input.GetKey(KeyCode.RightArrow))
        {
            transform.Rotate(Vector3.up * Time.deltaTime * tiltAngle, Space.World);
        }

        //transform.position -= zSpeed * transform.forward * Time.deltaTime;
        //transform.position += xSpeed * transform.right * Time.deltaTime;
        float moveHorizontal = Input.GetAxisRaw("Horizontal");
        float moveVertical = Input.GetAxisRaw("Vertical");
        Vector3 movement = new Vector3(moveHorizontal, 0.0f, moveVertical);
        if (movement != Vector3.zero){
            transform.rotation = Quaternion.LookRotation(movement);

        }


        if (Input.GetButton("Jump") && !mSpacePressed)
        {
            if (mCharges != 0)
            {
                cancel = false;
                chargeBar.SetActive(true);
                AkSoundEngine.PostEvent("Play_Charge", gameObject);
            }

        }

        mSpacePressed = Input.GetButton("Jump");

        if (mSpacePressed)
        {

            mTeleportTimer += Time.deltaTime;

        }



        if (mTeleportTimer > 3.0f)
        {

            //mSpacePressed = false;

            mTeleportTimer = 3.0f;

        }

        chargeBar.GetComponent<Slider>().value = mTeleportTimer / 3.0f;

        charges.text = "Teleport Charges: " + mCharges.ToString();

        if (Input.GetKey(KeyCode.JoystickButton7) || Input.GetKey(KeyCode.LeftShift)){
            mTeleportTimer = 0.0f;
            chargeBar.SetActive(false);
            AkSoundEngine.PostEvent("Stop_Charge", gameObject);
            cancel = true;
        }

        if (Input.GetButtonUp("Jump"))
        {
            if (mCharges != 0 && ! cancel)
            {

                transform.position += mTeleportTimer * transform.forward;

                mTeleportTimer = 0.5f;
                snapCamTimer = 0.0f;

                teleportEffect.Emit(1000);
                isDark = !isDark;

                chargeBar.SetActive(false);

                mCharges--;

                AkSoundEngine.PostEvent("Stop_Charge", gameObject);


                if (useMetric)
                    metricManager.GetComponent<MetricManager>().AddToTeleport(1);

            }

            else
            {
                chargeBar.SetActive(false);

            }

        }

        if (isDark)
        {
            transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.black;
            //GetComponent<MeshRenderer>().material.color = Color.black;
            teleportEffect.GetComponent<ParticleSystemRenderer>().material.shader = darkParticles;
            playerState = PlayerState.Dark;
            AkSoundEngine.PostEvent("Set_State_Dark", gameObject);
            DarkStateText.SetActive(true);
            LightStateText.SetActive(false);
            RedStateText.SetActive(false);
        }

        else
        {
            transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.white;
            //GetComponent<MeshRenderer>().material.color = Color.gray;
            teleportEffect.GetComponent<ParticleSystemRenderer>().material.shader = lightParticles;
            playerState = PlayerState.Light;
            AkSoundEngine.PostEvent("Set_State_Light", gameObject);
            DarkStateText.SetActive(false);
            LightStateText.SetActive(true);
            RedStateText.SetActive(false);
        }



        lightArray = GameObject.FindGameObjectsWithTag("LightSource");

        inLight = false;

        foreach (GameObject lightSource in lightArray)
        {

            Light l = lightSource.GetComponent<Light>();

            LightController lc = lightSource.GetComponent<LightController>();
            Vector3 diff = transform.position - lightSource.transform.position;



            if (diff.magnitude < l.range)

            {

                float angle = Mathf.Acos(Vector3.Dot(l.transform.forward, diff) / (l.transform.forward.magnitude * diff.magnitude));

                if (angle * Mathf.Rad2Deg <= l.spotAngle / 2)

                {

                    Vector3 rayDir = Vector3.Normalize(l.transform.position - transform.position);

                    Ray r = new Ray(transform.position, rayDir);

                    int layerMask = ~(1 << 12);

                    RaycastHit hitArray;

                    if (Physics.Raycast(r, out hitArray, l.range, layerMask))

                    {

                        if (lc.inWall)

                        {

                            if (hitArray.collider == lc.mWall)

                            {
                                //TODO: placeholder for what to do when in the light

                                inLight = true;

                            }

                        }

                    }

                    else

                    {

                        //TODO: placeholder for what to do when in the light

                        inLight = true;

                    }

                }

            }

        }

        if (inLight)
        {

            if (isDark)
            {

                isVisible = true;
            }
            else
            {
                isVisible = false;
            }

        }

        else
        {

            if (!isDark)
            {

                isVisible = true;

            }
            else
            {
                isVisible = false;
            }

        }

        if (isVisible)
        {
            displayFlash = true;
            //transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.red;
            //GetComponent<MeshRenderer>().material.color = Color.red;
            if (playerState == PlayerState.Dark){
                playerState = PlayerState.Dark_Red;
                AkSoundEngine.PostEvent("Set_State_Dark_Red", gameObject);
                DarkStateText.SetActive(false);
                LightStateText.SetActive(false);
                RedStateText.SetActive(true);

            }
            else if (playerState == PlayerState.Light){
                playerState = PlayerState.Light_Red;
                AkSoundEngine.PostEvent("Set_State_Light_Red", gameObject);
                DarkStateText.SetActive(false);
                LightStateText.SetActive(false);
                RedStateText.SetActive(true);
            }
        }

        if (displayFlash)
        {
            Color Opaque = new Color(1, 1, 1, 1);
            flashImage.color = Color.Lerp(flashImage.color, Opaque, Time.deltaTime);
            if (flashImage.color.a >= 0.8) //Almost Opaque, close enough
            {
                displayFlash = false;
            }
        }
        if (!displayFlash)
        {
            Color Transparent = new Color(1, 1, 1, 0);
            flashImage.color = Color.Lerp(flashImage.color, Transparent, 20 * Time.deltaTime);
        }

        rb.velocity = Vector3.zero;
        rb.angularVelocity = Vector3.zero;

        snapCamTimer += Time.deltaTime;
        if (snapCamTimer > 0.2f){
            snapCamTimer = 0.2f;
        }
        if (snapCamTimer == 0.2f){
            GameObject.FindGameObjectWithTag("MainCamera").transform.position = new Vector3(transform.position.x, GameObject.FindGameObjectWithTag("MainCamera").transform.position.y, transform.position.z);
        }

    }

    private void OnTriggerEnter(Collider collision)
    {
        foreach (GameObject gj in GameObject.FindGameObjectsWithTag("Charge"))
        {
            if (collision.gameObject == gj)
            {
                AkSoundEngine.PostEvent("Trigger_Charge_Pickup", gameObject);

                mCharges += 1;

                collision.gameObject.SetActive(false);
                Destroy(collision.gameObject);

            }
        }
        if (collision.gameObject.layer == 9)
        {
            transform.position = lastPos;
            //Ray r = new Ray(transform.position, -transform.forward);
            //RaycastHit hitArray;
            //if (Physics.Raycast(r, out hitArray, 10)){
            //    transform.position = hitArray.point;
            //}
        }
    }

}

