using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class PlayerMove : MonoBehaviour
{
    //public GameManager GameManager;
    public Vector3 moveForce;
    public float strafeSpeed;
    public Vector3 jumpForce;
    private bool _canJump = false;
    private bool _isShielded = false;
    public bool _isShieldOnCooldown = false;
    public float shieldDuration;
    public float shieldCooldown;
    public GameObject shield;
    public int collisionCount;

    private float _shieldDuration;
    private float _shieldCooldown;
    private bool _isInvulnerable = false;
    private bool _hasShownFlash = false;
    private float _invulnerableWindow = 2.0f;

    private Vector3 _initDash;
    private bool _isDashing = false;
    private float _dashT = 0.0f;
    private Rigidbody rb;
    public int score = 0;
    public AudioSource[] sounds;
    public AudioSource hitSound;
    public AudioSource collectSound;
    public AudioSource dashSound;
    public ParticleSystem shrink;

    // Start is called before the first frame update
    void Start()
    {
        sounds = GetComponents<AudioSource>();
        hitSound = sounds[1];
        collectSound = sounds[0];
        dashSound = sounds[2];

        rb = GetComponent<Rigidbody>();
        _shieldCooldown = shieldCooldown;
        _shieldDuration = shieldDuration;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        Vector3 dest = Vector3.zero;
        GameObject tempWell = null;
        LineRenderer line = GetComponent<LineRenderer>();
        foreach (var well in GameManager.wells)
        {
            if ((well.transform.position - transform.position).magnitude < 30.0f)
            {
                if (well.transform.position.z > transform.position.z)
                {
                    dest = well.transform.position;
                    well.GetComponent<InitWell>().emitter.SetActive(true);
                    Vector3[] posArray = {transform.position, dest};
                    line.enabled = true;
                    line.SetPositions(posArray);
                    tempWell = well;
                    break;
                }

            }
        }

        if (tempWell == null) line.enabled = false;
        if (_isDashing && dest != Vector3.zero)
        {
            dashSound.Play();
            _dashT += Time.deltaTime;
            float t = _dashT / 0.5f;
            transform.position = Vector3.Lerp(_initDash, dest, t);
            if (t > 1.0f)
            {
                _isDashing = false;
                score += 10;
                tempWell.GetComponent<InitWell>().explode = true;
            }
            
        }
        else
        {
            transform.position += moveForce * Time.deltaTime;
            float horizontal = Input.GetAxis("Horizontal");
            Vector3 strafe = new Vector3(horizontal,0.0f,0.0f);
            transform.position += strafeSpeed * strafe * Time.deltaTime;
        }
        //if (transform.position.y < -4)
        //{
        //    SceneManager.LoadScene(0);
        //}

        if (Input.GetKeyDown(KeyCode.JoystickButton0) || Input.GetKeyDown(KeyCode.R))
        {
            SceneManager.LoadScene(1);
        }

        if ((Input.GetKeyDown(KeyCode.JoystickButton1) || Input.GetKeyDown(KeyCode.Space)) && _canJump)
        {
            //Debug.Log("CAN JUMP Space is pressed");
            rb.AddForce(jumpForce, ForceMode.Impulse);
            _canJump = false;
        }

        if ((Input.GetKeyDown(KeyCode.JoystickButton5) || Input.GetKeyDown(KeyCode.Q)) && !_isShieldOnCooldown && !_isShielded)
        {
            _isShielded = true;
            shieldDuration = _shieldDuration;
        }

        if (Input.GetKeyDown(KeyCode.JoystickButton4) || Input.GetKeyDown(KeyCode.LeftShift))
        {
            _isDashing = true;
            _initDash = transform.position;
        }

        if (_isShielded)
        {
            shieldDuration -= Time.deltaTime;
            if (shieldDuration < 0.0f)
            {
                _isShielded = false;
                _isShieldOnCooldown = true;
                shieldCooldown = _shieldCooldown;
            }
        }

        if (_isShieldOnCooldown)
        {
            shieldCooldown -= Time.deltaTime;
            if (shieldCooldown < 0.0f)
            {
                _isShieldOnCooldown = false;
            }
        }
        shield.SetActive(_isShielded);
        if (_isInvulnerable)
        {
            if (!_hasShownFlash)
            {
                InvokeRepeating("flash",0,0.2f);
                _hasShownFlash = true;
            }

            _invulnerableWindow -= Time.deltaTime;
            if (_invulnerableWindow < 0.0f)
            {
                _isInvulnerable = false;
                _hasShownFlash = false;
                _invulnerableWindow = 2.0f;
                CancelInvoke("flash");
            }
        }
    }

    private void OnCollisionEnter(Collision other)
    {
        if (other.collider.CompareTag("Ground"))
        {

            _canJump = true;
        }

        if (other.collider.CompareTag("Block") && !_isShielded)
        {
            hitSound.Play();
            Destroy(other.gameObject);
            if (!_isInvulnerable)
            {
                CameraShake.Shake(0.25f,1.0f);
                shrink.Emit(100);
                collisionCount++;
                transform.localScale = new Vector3((5-collisionCount)/10.0f + 0.5f, (5-collisionCount)/10.0f + 0.5f,(5-collisionCount)/10.0f + 0.5f);
                _isInvulnerable = true;
            }
            if (collisionCount >= 5)
            {
                SceneManager.LoadScene(1);
            }
        }
        if (other.collider.CompareTag("Collectible"))
        {
            collectSound.Play();
            Destroy(other.gameObject);
            score += 2;
        }
        if (other.collider.CompareTag("Stackable"))
        {
            collisionCount++;
            transform.position += new Vector3(0, 1, 1.2f) ;
        }

    }

    private void flash()
    {
        GetComponent<MeshRenderer>().enabled = !GetComponent<MeshRenderer>().enabled;
        
        
    }

    //private void OnCollisionExit(Collision other)
    //{
    //    if (other.collider.CompareTag("Ground"))
    //    {
    //        _canJump = false;
    //    }   
    //}
}
